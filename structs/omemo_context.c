#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <key_helper.h>
#include <signal_protocol.h>
#include <omemo/omemo_constants.h>
#include <store/omemo_store.h>
#include <crypto/crypto_provider.h>
#include <structs/omemo_context.h>

#define UNUSED(x) (void)(x)

static void omemo_lock(void *user_data)
{
	struct omemo_context *context = user_data;

	pthread_mutex_lock(&context->mutex);
}

static void omemo_unlock(void *user_data)
{
	struct omemo_context *context = user_data;

	pthread_mutex_unlock(&context->mutex);
}

struct omemo_context *omemo_context_create(const signal_protocol_address *own_address)
{
	int retval;
	struct omemo_context *context;

	if (!own_address) {
		errno = EINVAL;
		return NULL;
	}

	context = malloc(sizeof(struct omemo_context));
	if (!context) {
		errno = ENOMEM;
		return NULL;
	}

	memcpy(&context->own_address, own_address, sizeof(signal_protocol_address));
	context->store_context = omemo_store_context_create(context);
	if (!context->store_context) {
		return NULL;
	}

	retval = signal_context_create(&context->signal_ctx, context);
	if (retval < 0) {
		errno = ENOMEM;
		return NULL;
	}

	retval = signal_context_set_crypto_provider(context->signal_ctx, &omemo_crypto_provider);
	if (retval < 0) {
		return NULL;
	}

	pthread_mutex_init(&context->mutex, NULL);
	retval = signal_context_set_locking_functions(context->signal_ctx, omemo_lock, omemo_unlock);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_create(&context->signal_store_ctx, context->signal_ctx);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_session_store(context->signal_store_ctx,
	                &context->store_context->session_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_pre_key_store(context->signal_store_ctx,
	                &context->store_context->pre_key_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_signed_pre_key_store(context->signal_store_ctx,
	                &context->store_context->signed_pre_key_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_identity_key_store(context->signal_store_ctx,
	                &context->store_context->identity_key_store);
	if (retval < 0) {
		return NULL;
	}

	return context;
}

int omemo_context_install(struct omemo_context *ctx)
{
	int retval;
	struct timeval tv;
	uint64_t msecs_epoch;
	signal_context *signal_ctx;
	signal_protocol_store_context *store_ctx;
	ratchet_identity_key_pair *identity_key_pair;
	signal_protocol_key_helper_pre_key_list_node *pre_keys;
	signal_protocol_key_helper_pre_key_list_node *it;
	session_signed_pre_key *signed_pre_key;
	session_pre_key *cur;

	//	if (!ctx) {
	//		errno = EINVAL;
	//		return -1;
	//	}

	signal_ctx = ctx->signal_ctx;
	store_ctx = ctx->signal_store_ctx;

	retval = signal_protocol_key_helper_generate_identity_key_pair(&identity_key_pair, signal_ctx);
	if (retval < 0) {
		errno = ENOKEY;
		return -1;
	}

	retval = signal_protocol_key_helper_generate_pre_keys(&pre_keys, 0, OMEMO_NUM_PRE_KEYS, signal_ctx);
	if (retval < 0) {
		errno = ENOKEY;
		return -1;
	}

	gettimeofday(&tv, NULL);
	msecs_epoch = (uint64_t) (tv.tv_sec) * 1000 + (uint64_t) (tv.tv_usec) / 1000;
	retval = signal_protocol_key_helper_generate_signed_pre_key(&signed_pre_key, identity_key_pair, 0,
	                msecs_epoch, signal_ctx);
	if (retval < 0) {
		errno = ENOKEY;
		return -1;
	}

	/* Store local identity key pair.
	 * --> HERE
	 */

	retval = signal_protocol_signed_pre_key_store_key(store_ctx, signed_pre_key);
	if (retval < 0) {
		errno = EIO;
		fputs("Error when storing signed_pre_key\n", stderr);
		return -1;
	}

	/* Store all pre keys */
	for (it = pre_keys; it; it = signal_protocol_key_helper_key_list_next(it)) {
		cur = signal_protocol_key_helper_key_list_element(it);
		retval = signal_protocol_pre_key_store_key(store_ctx, cur);
		if (retval < 0) {
			fputs("Error when storing pre_key\n", stderr);
			errno = EIO;
			return -1;
		}
	}

	/* TODO: Publish device list and bundle.
	 * The list of pre keys should be added here to the omemo bundle and then be maintained from there.
	 */

	return 0;

}

void omemo_context_free(struct omemo_context *context)
{
	if (!context) {
		return;
	}

	signal_protocol_store_context_destroy(context->signal_store_ctx);
	signal_context_destroy(context->signal_ctx);
	pthread_mutex_destroy(&context->mutex);
	free(context->store_context);
	free(context);
}

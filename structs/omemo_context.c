#include <errno.h>
#include <string.h>
#include <signal_protocol.h>
#include <crypto/crypto_provider.h>
#include <store/omemo_store.h>
#include <omemo/omemo.h>

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

#include <errno.h>
#include <key_helper.h>
#include <signal_protocol.h>
#include <structs/omemo_context.h>
#include <sys/time.h>
#include <xmpp/omemo_constants.h>

int omemo_install(struct omemo_context *context)
{
	int retval;
	struct timeval tv;
	uint64_t msecs_epoch;
	signal_context *ctx;
	signal_protocol_store_context *store_ctx;
	ratchet_identity_key_pair *identity_key_pair;
	signal_protocol_key_helper_pre_key_list_node *pre_keys;
	signal_protocol_key_helper_pre_key_list_node *it;
	session_signed_pre_key *signed_pre_key;
	session_pre_key *cur;

	if (!context) {
		errno = EINVAL;
		return -1;
	}

	ctx = context->signal_ctx;
	store_ctx = context->signal_store_ctx;

	retval = signal_protocol_key_helper_generate_identity_key_pair(&identity_key_pair, ctx);
	if (retval < 0) {
		errno = ENOKEY;
		return -1;
	}

	retval = signal_protocol_key_helper_generate_pre_keys(&pre_keys, 0, OMEMO_NUM_PRE_KEYS, ctx);
	if (retval < 0) {
		errno = ENOKEY;
		return -1;
	}
	
	gettimeofday(&tv, NULL);
	msecs_epoch = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
	retval = signal_protocol_key_helper_generate_signed_pre_key(&signed_pre_key, identity_key_pair, 0,
								    msecs_epoch, ctx);
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

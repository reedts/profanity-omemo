#include <errno.h>
#include <string.h>
#include <signal_protocol.h>
#include <crypto/crypto_provider.h>
#include <store/omemo_store.h>
#include "omemo_context.h"


struct omemo_context *omemo_context_create(const signal_protocol_address *address)
{
	int retval;
	struct omemo_context *context;

	if (!address) {
		errno = EINVAL;
		return NULL;
	}

	context = malloc(sizeof(struct omemo_context));
	if (!context) {
		errno = ENOMEM;
		return NULL;
	}
	
	memcpy(&context->own_address, address, sizeof(signal_protocol_address));
	context->store_context = &omemo_store_context;

	/* Check if the user's context exists already */
	if (!omemo_is_local_user_existent(address)) {
		/* Install OMEMO, a.k.a generate Keys etc. */
	}

	retval = signal_context_create(&context->signal_context, NULL);
	if (retval < 0) {
		errno = ENOMEM;
		return NULL;
	}

	retval = signal_context_set_crypto_provider(context->signal_context, &omemo_crypto_provider);
	if (retval < 0) {
		return NULL;
	}

	/* TODO: Set signal locking functions */

	retval = signal_protocol_store_context_create(&context->signal_store_context, context->signal_context);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_session_store(context->signal_store_context,
			&omemo_store_context.session_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_pre_key_store(context->signal_store_context,
			&omemo_store_context.pre_key_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_signed_pre_key_store(context->signal_store_context,
			&omemo_store_context.signed_pre_key_store);
	if (retval < 0) {
		return NULL;
	}

	retval = signal_protocol_store_context_set_identity_key_store(context->signal_store_context,
			&omemo_store_context.identity_key_store);
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

	signal_protocol_store_context_destroy(context->signal_store_context);
	signal_context_destroy(context->signal_context);

	free(context);
}

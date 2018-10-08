//
// Created by roobre on 2/18/18.
//

#include <string.h>
#include <signal_protocol.h>
#include <omemo/omemo.h>
#include <omemo/omemo_constants.h>
#include <structs/omemo_context.h>
#include <structs/omemo_context_def.h>
#include <structs/omemo_context_global.h>
#include <xmpp/omemo_stanza.h>
#include <gcrypt.h>
#include <store/omemo_store.h>

struct omemo_context_global ctx;

void omemo_init(void)
{
	ctx.logger(OMEMO_LOGLVL_INFO, "Global context initialized.");
	// TODO: Initialize stuff
}

omemo_context *omemo_init_account(const char *barejid)
{
	signal_protocol_address addr;
	addr.name_len = strlen(barejid);
	addr.name = strcpy(malloc(addr.name_len), barejid);
	// XEP dictates device_id must be a random, non-zero, positive 32-bit integer.
	do {
		gcry_create_nonce(&addr.device_id, sizeof(addr.device_id));
	} while (addr.device_id == 0); // Retry if we were extremely unlucky
	if (addr.device_id < 0) // Flip sign if negative, doesn't have an impact in randomness
		addr.device_id *= -1;

	omemo_context *userctx = omemo_context_create(&addr);

	if (!omemo_is_local_user_existent(&userctx->own_address)) {
		omemo_context_install(userctx);
	}

	char *str = malloc(31 + 1 + strlen(barejid)); // !! 31 is hardcoded
	sprintf(str, "Context registered for account %s", barejid);
	ctx.logger(OMEMO_LOGLVL_INFO, str);
	free(str);


	// TODO: Announce omemo support

	return userctx;
}

int omemo_is_stanza(const char *stanza)
{
	return strstr(stanza, OMEMO_XML_NS) != NULL;
}

char *omemo_send_encrypted(omemo_context *context, const char *msg_stanza)
{
	// TODO: Actual encryption
	ctx.logger(OMEMO_LOGLVL_DEBUG, "Encrypting message");
	// Display original message

	// TODO: Compose a proper OMEMO msg_stanza
	return NULL;
}

char *omemo_receive_encrypted(omemo_context *context, const char *msg_stanza)
{
	if (omemo_check_stanza_type(msg_stanza) != OMEMO_STYPE_NONE) {
		// This is an OMEMO stanza, process it
		// TODO: Actual encryption
		// ctx.logger(OMEMO_LOGLVL_DEBUG, "Decrypting message");
		return strcpy(malloc(64), "(TODO) Encrypted OMEMO message");
	}

	return NULL;
}

void omemo_free_account(omemo_context *context)
{
	omemo_context_free(context);
}

void omemo_shutdown()
{
}

void omemo_set_logger(omemo_logger logger)
{
	ctx.logger = logger;
}

void omemo_message_free(char *ptr)
{
	free(ptr);
}

//
// Created by roobre on 2/18/18.
//

#include <signal_protocol.h>
#include <omemo/omemo.h>
#include <omemo/omemo_constants.h>
#include <structs/omemo_context.h>
#include <xmpp/omemo_stanza.h>
#include <string.h>

struct omemo_context_global ctx;

void omemo_init(void)
{
	ctx.omemo_user_contexts = calloc(1, sizeof(struct omemo_context *));
	// TODO: Initialize stuff
}

int omemo_init_account(const char *barejid)
{
	signal_protocol_address addr;
	addr.name_len = strlen(barejid);
	addr.name = strcpy(malloc(addr.name_len), barejid);
	addr.device_id = 0; // TODO

	ctx.omemo_user_contexts[0] = omemo_context_create(&addr);

	return 0;
}

char *omemo_send_encrypted(const char *msg_stanza)
{
	// TODO: Actual encryption
	//    ctx.logger(OMEMO_LOGLVL_DEBUG, "Encrypting message");
	// Display original message
	//    ctx.msg_displayer(receiver_jid, msg_stanza);

	// TODO: Compose a proper OMEMO msg_stanza
	return NULL;
}

char *omemo_receive_encrypted(const char *msg_stanza)
{
	if (omemo_check_stanza_type(msg_stanza) != OMEMO_STYPE_NONE) {
		// This is an OMEMO stanza, process it
		// TODO: Actual encryption
		// ctx.logger(OMEMO_LOGLVL_DEBUG, "Decrypting message");
		return strcpy(malloc(64), "(TODO) Encrypted OMEMO message");
	}

	return NULL;
}

void omemo_shutdown(void)
{
	omemo_context_free(ctx.omemo_user_contexts[0]);
	free(ctx.omemo_user_contexts);
}

void omemo_set_logger(omemo_logger logger)
{
	ctx.logger = logger;
}

void omemo_message_free(char *ptr)
{
	free(ptr);
}

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
	ctx.omemo_user_contexts = calloc(sizeof(struct omemo_context *), 1);
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

char *omemo_send_encrypted(const char *barejid, const char *receiver_jid, const char *msg_stanza)
{
	// TODO: Actual encryption
	//    ctx.logger(OMEMO_LOGLVL_DEBUG, "Encrypting message");
	// Display original message
	//    ctx.msg_displayer(receiver_jid, msg_stanza);

	// TODO: Compose a proper OMEMO msg_stanza
	return NULL;
}

char *omemo_receive_encrypted(const char *barejid, const char *sender_jid, const char *stanza)
{
	if (omemo_check_stanza_type(stanza) != OMEMO_STYPE_NONE) {
		// This is an OMEMO stanza, process it
		// TODO: Actual encryption
		// ctx.logger(OMEMO_LOGLVL_DEBUG, "Decrypting message");
	} else {
		// Return a copy of it
		return strcpy(malloc(strlen(stanza)), stanza);
	}

	return NULL;
}

void omemo_shutdown(void)
{
	omemo_context_free(ctx.omemo_user_contexts[0]);
	free(ctx.omemo_user_contexts);
}

void omemo_set_msg_displayer(omemo_msg_displayer msg_displayer)
{
	ctx.msg_displayer = msg_displayer;
}

void omemo_set_stanza_sender(omemo_stanza_sender stanza_sender)
{
	ctx.stanza_sender = stanza_sender;
}

void omemo_set_logger(omemo_logger logger)
{
	ctx.logger = logger;
}

void omemo_message_free(const char *ptr)
{

}

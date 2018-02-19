//
// Created by roobre on 2/18/18.
//

#include <omemo/omemo.h>

// TODO: Write generic hooks and callbacks to make this client-agnostic.
// Meanwhile, we'll just call profanity functions from here.
#include <profapi.h>
#include <structs/omemo_context.h>

struct omemo_context ctx;

void omemo_hook_init()
{
// TODO: Initialize stuff
}

void omemo_hook_add_account(const char *barejid)
{
// TODO: Create context for barejid
}

void omemo_hook_encrypt(const char *barejid, const char *receiver_jid, const char *message)
{
	// TODO: Actual encryption
	ctx.logger(OMEMO_LOGLVL_DEBUG, "Encrypting message");
	// Display original message
	ctx.msg_displayer(receiver_jid, message);
	// TODO: Compose a proper OMEMO stanza
	ctx.stanza_sender(message);
}

char *omemo_hook_decrypt(const char *barejid, const char *sender_jid, const char *ciphertext)
{
	// TODO: Actual encryption
	ctx.logger(OMEMO_LOGLVL_DEBUG, "Decrypting message");
	return ciphertext;
}

void omemo_hook_shutdown()
{
	// TODO: Something?
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

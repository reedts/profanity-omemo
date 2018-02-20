//
// Created by roobre on 2/18/18.
//

#ifndef PROFANITY_OMEMO_MAIN_H
#define PROFANITY_OMEMO_MAIN_H

#include <omemo/omemo_types.h>

/**
 * Called only once to initialize internal states, typically when the plugin is loaded.
 */
void omemo_init(void);

/**
 * Initializes the account identified by the given JID. Key material and sessions will be loeaded from disk if possible,
 * newly created otherwise.
 *
 * @param barejid JID of the user to initialize the context for
 */
int omemo_init_account(const char *barejid);

/**
 * Returns the omemo-equivalent of the given stanza. If not applicable, it will return NULL.
 * This is safe to be called on every outgoing stanza.
 * @param msg_stanza Message to be encrypted.
 * @return Omemo stanza if applicable, NULL otherwise.
 */
char *omemo_send_encrypted(const char *msg_stanza);

/**
 * Decrypts the given stanza. Returns the message in plaintext, or NULL if it could not be decrypted.
 * @param stanza Message to be decrypted
 * @return THe message in plaintext, or NULL if it couldn't be decrypted.
 */
char *omemo_receive_encrypted(const char *stanza);

/**
 * Called when the client is shutting down
 */
void omemo_shutdown(void);

/**
 * Sets the stanza sender, a function capable of sending an XMPP stanza.
 * @param stanza_sender Function capable of sending an XMPP stanza.
 */
void omemo_set_logger(omemo_logger logger);

void omemo_message_free(char *ptr);

#endif //PROFANITY_OMEMO_MAIN_H

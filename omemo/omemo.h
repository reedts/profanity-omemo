//
// Created by roobre on 2/18/18.
//

#ifndef PROFANITY_OMEMO_MAIN_H
#define PROFANITY_OMEMO_MAIN_H

#include <omemo/omemo_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Called only once to initialize internal states, typically when the plugin is loaded.
 */
void omemo_init(void);

/**
 * Initializes the account identified by the given JID. Key material and sessions will be loaded from disk if possible,
 * newly created otherwise. This is safe to be called repeatedly. Will set an OMEMO context accordingly to be used with these
 * high-level library routines.
 *
 * @param barejid JID of the user to initialize the context for
 */
omemo_context *omemo_init_account(const char *barejid);

/**
 * Checks if the supplied stanza is suitable to be processed by the omemo library.
 * @param stanza Stanza to be checked.
 * @return 1 if it is OMEMO-related, 0 otherwise.
 */
int omemo_is_stanza(const char *stanza);

/**
 * Returns the omemo-equivalent of the given stanza. If not applicable, it will return NULL.
 * This is safe to be called on every outgoing stanza.
 * @param msg_stanza Message to be encrypted.
 * @param OMEMO context to use
 * @return Omemo stanza if applicable, NULL otherwise.
 */
char *omemo_send_encrypted(omemo_context *context, const char *msg_stanza);

/**
 * Processes the given stanza. If it was an OMEMO message stanza and could be decrypted, returns the plaintext message.
 *  If it was another type of OMEMO-related stanza, it is processed but NULL is returned.
 * @param stanza Stanza to be processed. If it is not an OMEMO stanza, it will be ignored.
 * @param OMEMO context to use
 * @return A pointer to the decrypted message if it was a message and could be decrypted, NULL otherwise.
 */
char *omemo_receive_encrypted(omemo_context *context, const char *stanza);

/**
 * Frees all resources allocated for the provided OMEMO context.
 * @param OMEMO context to deallocate
 */
void omemo_free_account(omemo_context *context);

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

#ifdef __cplusplus
}
#endif

#endif //PROFANITY_OMEMO_MAIN_H

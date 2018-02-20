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
 * Encrypts the given stanza.
 * Encrypted stanza will be sent using stanza_sender and plaintext message will be displayed using message_displayer.
 * @param barejid JID of the account sending the message.
 * @param receiver_jid Receiver's JID.
 * @param msg_stanza Message to be encrypted.
 */
char *omemo_send_encrypted(const char *barejid, const char *receiver_jid, const char *msg_stanza);

/**
 * Decrypts the given stanza
 * @param barejid JID of the account receiving the message.
 * @param sender_jid JID of the message sender.
 * @param ciphertext_stanza Message to be decrypted
 * @return The decrypted message, ready to be displayed to the user.
 */
char *omemo_receive_encrypted(const char *barejid, const char *sender_jid, const char *stanza);

/**
 * Called when the client is shutting down
 */
void omemo_shutdown(void);

/**
 * Sets the message shower, a function capable of displaying a message in the chat window.
 * This will be called when an encrypted message is sent, to display the readable version to the user in the chat window.
 * @param msg_shower Function capable of displaying a message in the user interface.
 */
void omemo_set_msg_displayer(omemo_msg_displayer msg_displayer);

/**
 * Sets the stanza sender, a function capable of sending an XMPP stanza.
 * @param stanza_sender Function capable of sending an XMPP stanza.
 */
void omemo_set_logger(omemo_logger logger);

void omemo_message_free(const char *ptr);

#endif //PROFANITY_OMEMO_MAIN_H

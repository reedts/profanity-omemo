//
// Created by roobre on 2/18/18.
//

#ifndef PROFANITY_OMEMO_MAIN_H
#define PROFANITY_OMEMO_MAIN_H

#include <structs/omemo_context.h>

/**
 * Installation routine for the plugin.
 * This is only called one single time, when a local account adds OMEMO support.
 * It generates the needed keys and saves them.
 *
 * @param context The context of the local user to install the plugin
 */
void omemo_install(struct omemo_context *context);

/**
 * Called only once to initialize internal states, typically when the plugin is loaded.
 */
void omemo_hook_init();

/**
 * Registers the given JID into the plugin structures.
 * This allows later calls to encrypt, decrypt, subscribe etc related to this account
 *
 * @param barejid The JID of the account to be registered
 */
void omemo_hook_add_account(const char *barejid);

/**
 * Encrypts the given message.
 * Encrypted stanza will be sent using stanza_sender and plaintext message will be displayed using message_displayer.
 * @param barejid JID of the account sending the message.
 * @param receiver_jid Receiver's JID.
 * @param message Message to be encrypted.
 */
void omemo_hook_encrypt(const char *barejid, const char *receiver_jid, const char *message);

/**
 * Decrypts the given ciphertext
 * @param barejid JID of the account receiving the message.
 * @param sender_jid JID of the message sender.
 * @param ciphertext Message to be decrypted
 * @return The decrypted message, ready to be displayed to the user.
 */
char *omemo_hook_decrypt(const char *barejid, const char *sender_jid, const char *ciphertext);

/**
 * Called when the client is shutting down
 */
void omemo_hook_shutdown();

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
void omemo_set_stanza_sender(omemo_stanza_sender stanza_sender);

/**
 * Sets the stanza sender, a function capable of sending an XMPP stanza.
 * @param stanza_sender Function capable of sending an XMPP stanza.
 */
void omemo_set_logger(omemo_logger logger);

void omemo_message_free(const char *ptr);

#endif //PROFANITY_OMEMO_MAIN_H

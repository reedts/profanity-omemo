//
// Created by roobre on 2/18/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <omemo/omemo.h>
#include <string.h>
#include <profapi.h>
#include <omemo/omemo.h>

#define CMD_NAME "omemo"

const char *const DEVICELIST_FEATURE_NAME = "eu.siacs.conversations.axolotl.devicelist";

const char *const LOG_PREFIX = "[OMEMO] ";

omemo_context *context;


void omemo_logger_profanity(omemo_log_level lvl, const char *message)
{
	char *prefixedmsg = strcpy(malloc(strlen(LOG_PREFIX) + strlen(message) + 1), LOG_PREFIX);
	strcat(prefixedmsg, message);

	switch (lvl) {
	case OMEMO_LOGLVL_DEBUG:
		prof_log_debug(prefixedmsg);
		break;
	case OMEMO_LOGLVL_INFO:
		prof_log_info(prefixedmsg);
		break;
	case OMEMO_LOGLVL_WARNING:
		prof_log_warning(prefixedmsg);
		break;
	case OMEMO_LOGLVL_ERROR:
		prof_log_error(prefixedmsg);
		break;
	}

	free(prefixedmsg);
}

void omemo_cmd(char **args)
{
	omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "/" CMD_NAME " command called.");

	// Profanity API guarantees this won't be called for less than 1 argument
	if (strcmp(args[0], "help") == 0) {
		// Undocumented but probably useful for someone
		prof_send_line("/help omemo");
	} else if (strcmp(args[0], "register") == 0) {

	} else {
		prof_cons_show(CMD_NAME ": Unknown command");
	}
}

const char *jid_trim(char *fulljid)
{
	if (strchr(fulljid, '/') == NULL) {
		return fulljid;
	} else {
		return strtok(fulljid, "/");
	}
}

void prof_init(const char *const version,
               const char *const status,
               const char *const account_name,
               const char *const fulljid
              )
{
	omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "Loading plugin.");

	char *synopsis[] = {
		"/" CMD_NAME " register <account>",
		"/" CMD_NAME " start [jid]",
		"/" CMD_NAME " stop",
		"/" CMD_NAME " list-keys [jid]",
		"/" CMD_NAME " trust <keyid>",
		"/" CMD_NAME " untrust <keyid>",
		NULL
	};

	char *arguments[][2] = {
		{"register",  "Generates keys and settings for the given account."},
		{"start",     "Enables omemo encryption for the given JID, or the current conversation."},
		{"stop",      "Disables omemo encryption for the current conversation."},
		{"list-keys", "List known omemo keys, along with its trust status."},
		{"trust",     "Marks the specified key as trusted. Key is automatically using the first chars if not ambiguous."},
		{"untrust",   "Marks the specified key as untrusted."},
		{NULL, NULL}
	};

	char *examples[] = {
		"/" CMD_NAME " register roobre@roobre.es",
		"/" CMD_NAME " start reedts@trashserver.net",
		"/" CMD_NAME " stop",
		"/" CMD_NAME " trust 03B",
		"/" CMD_NAME " untrust 03B26626",
		NULL
	};


	omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "Registering commands");

	prof_register_command("/"
	                      CMD_NAME, 1, 2, synopsis,
	                      "Changes OMEMO settings and starts OMEMO sessions", arguments, examples,
	                      omemo_cmd
	                     );

	omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "Registering loggers.");

	omemo_set_logger(omemo_logger_profanity);

	omemo_init();
	if (fulljid != NULL) {
		char *jid = strcpy(malloc(strlen(fulljid)), fulljid);
		context = omemo_init_account(jid_trim(jid));
		free(jid);
	}
}

void prof_on_connect(const char *const account_name,
                     const char *const fulljid)
{
	/*
	 * TODO: Necessary?
	 * char *jid = strcpy(malloc(strlen(fulljid)), fulljid);
	 * omemo_init_account(jid_trim(jid));
	 * free(jid);
	 */

	char *feature = strcpy(malloc(strlen(DEVICELIST_FEATURE_NAME)), DEVICELIST_FEATURE_NAME);
	prof_disco_add_feature(feature);
	free(feature); // TODO: Check if profanity frees this for us
}

char *prof_on_message_stanza_send(const char *const stanza)
{
	omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "Sending stanza");
	return omemo_send_encrypted(context, stanza);
}

int on_stanza_receive(const char *const stanza)
{
	if (omemo_is_stanza(stanza)) {
		omemo_logger_profanity(OMEMO_LOGLVL_DEBUG, "Stanza received");

		// Process stanza and return message if available
		char *message = omemo_receive_encrypted(context, stanza);
		if (message != NULL) {
			// Stanza was a message and could be decrypted
			char *sender = strcpy(malloc(64), "nobody@example.org"); // TODO: Get from stanza
			prof_chat_show(sender, message);
		}

		// Stanza processed, tell profanity to discard it.
		return 0;
	} else {
		// We're not interested on this, tell profanity to keep scanning it.
		return 1;
	}
}

int prof_on_message_stanza_receive(const char *const stanza)
{
	return on_stanza_receive(stanza);
}

int prof_on_iq_stanza_receive(const char *const stanza)
{
	return on_stanza_receive(stanza);
}

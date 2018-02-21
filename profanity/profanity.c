//
// Created by roobre on 2/18/18.
//

#include <stdlib.h>
#include <omemo/omemo.h>
#include <string.h>
#include <profapi.h>

#define CMD_NAME "omemo"

void omemo_logger_profanity(omemo_log_level lvl, const char *message)
{
	switch (lvl) {
	case OMEMO_LOGLVL_DEBUG:
		prof_log_debug(message);
		break;
	case OMEMO_LOGLVL_INFO:
		prof_log_debug(message);
		break;
	case OMEMO_LOGLVL_WARNING:
		prof_log_debug(message);
		break;
	case OMEMO_LOGLVL_ERROR:
		prof_log_debug(message);
		break;
	}
}

void omemo_cmd(char **args)
{
	prof_log_info("[OMEMO] command called");
	if (args[0] != NULL && strcmp(args[0], "help") == 0) {
		prof_send_line("/help omemo");
	}
}

void prof_init(const char *const version,
               const char *const status,
               const char *const account_name,
               const char *const fulljid
              )
{
	prof_log_info("[OMEMO] Loading plugin.");

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
		{"register", "Generates keys and settings for the given account."},
		{"start", "Enables omemo encryption for the given JID, or the current conversation."},
		{"stop", "Disables omemo encryption for the current conversation."},
		{"list-keys", "List known omemo keys, along with its trust status."},
		{"trust", "Marks the specified key as trusted. Key is automatically using the first chars if not ambiguous."},
		{"untrust", "Marks the specified key as untrusted."},
		{NULL, NULL}
	};

	char *examples[] = {
		"/" CMD_NAME " register roobre@roobre.es",
		"/" CMD_NAME " start reedts@trashserver.net",
		"/" CMD_NAME " stop",
		"/" CMD_NAME " trust 03B26626[3C571553[...]]",
		NULL
	};

	prof_log_info("[OMEMO] Registering commands");

	prof_register_command("/" CMD_NAME, 1, 2, synopsis,
	                      "Changes OMEMO settings and starts OMEMO sessions", arguments, examples,
	                      omemo_cmd
	                     );

	prof_log_info("[OMEMO] Registering loggers,");

	omemo_set_logger(omemo_logger_profanity);
	omemo_init();
	if (fulljid != NULL) {
		omemo_init_account(fulljid);
	}
}

char *prof_on_message_stanza_send(const char *const stanza)
{
	return omemo_send_encrypted(stanza);
}

int prof_on_message_stanza_receive(const char *const stanza)
{
	char *sender = strcpy(malloc(64), "nobody@example.org"); // TODO: Get from stanza
	char *message = omemo_receive_encrypted(stanza);
	prof_chat_show(sender, message);
	return 0;
}

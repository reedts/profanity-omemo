//
// Created by roobre on 2/18/18.
//

#include <stdlib.h>
#include <omemo/omemo.h>
#include <string.h>
#include <profapi.h>

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

void prof_init(const char *const version,
               const char *const status,
               const char *const account_name,
               const char *const fulljid
              )
{
	prof_log_info("Profanity-omemo loaded!");

	omemo_init();
	omemo_set_logger(omemo_logger_profanity);
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

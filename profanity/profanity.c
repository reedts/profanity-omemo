//
// Created by roobre on 2/18/18.
//

#include <omemo/omemo.h>
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

void omemo_msg_displayer_profanity(char *receiver, char *msg)
{
	prof_chat_show(receiver, msg);
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
	omemo_set_msg_displayer(omemo_msg_displayer_profanity);
}

char *prof_pre_chat_message_send(const char *const barejid, const char *message)
{
	char *receiver = prof_get_current_recipient();
	omemo_send_encrypted(barejid, receiver, message);
	return NULL;
}

char *prof_pre_room_message_display(const char *const barejid, const char *const nick, const char *message)
{
	char *sender = prof_get_current_recipient();
	return omemo_receive_encrypted(barejid, sender, message);
}

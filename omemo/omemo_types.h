//
// Created by roobre on 2/20/18.
//

#ifndef PROFANITY_OMEMO_OMEMO_TYPES_H_H
#define PROFANITY_OMEMO_OMEMO_TYPES_H_H

typedef enum {
	OMEMO_LOGLVL_DEBUG = 0,
	OMEMO_LOGLVL_INFO,
	OMEMO_LOGLVL_WARNING,
	OMEMO_LOGLVL_ERROR
} omemo_log_level;

typedef void (* omemo_msg_displayer)(char *receiver, char *msg);

typedef void (* omemo_stanza_sender)(char *stanza);

typedef void (* omemo_logger)(omemo_log_level lvl, const char *message);

#endif //PROFANITY_OMEMO_OMEMO_TYPES_H_H

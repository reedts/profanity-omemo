//
// Created by roobre on 2/20/18.
//

#ifndef PROFANITY_OMEMO_OMEMO_STANZA_H
#define PROFANITY_OMEMO_OMEMO_STANZA_H

typedef enum {
	OMEMO_STYPE_NONE = 0, // < Not an omemo stanza
	OMEMO_STYPE_ENCRYPTED,
	OMEMO_STYPE_LIST,
	OMEMO_STYPE_BUNDLE
} omemo_stanza_type;

/**
 * Checks if the received stanza should be processed by the plugin
 * @param stanza Raw stanza
 * @return 1 if it should, 0 otherwise
 */
omemo_stanza_type omemo_check_stanza_type(const char *stanza);

#endif //PROFANITY_OMEMO_OMEMO_INTERNALS_H

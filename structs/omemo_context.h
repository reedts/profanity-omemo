#ifndef PROF_OMEMO_CONTEXT_H
#define PROF_OMEMO_CONTEXT_H

#include <pthread.h>
#include <signal_protocol_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	OMEMO_LOGLVL_DEBUG = 0,
	OMEMO_LOGLVL_INFO,
	OMEMO_LOGLVL_WARNING,
	OMEMO_LOGLVL_ERROR
} omemo_log_level;

typedef void (* omemo_msg_displayer)(char *receiver, char *msg);

typedef void (* omemo_stanza_sender)(char *stanza);

typedef void (* omemo_logger)(omemo_log_level lvl, const char *message);

struct omemo_context {
	signal_protocol_address own_address;
	signal_context *signal_ctx;
	signal_protocol_store_context *signal_store_ctx;

	struct omemo_store_context *store_context;
	struct omemo_bundle *bundle;

	omemo_msg_displayer msg_displayer;
	omemo_stanza_sender stanza_sender;
	omemo_logger logger;

	pthread_mutex_t mutex;
};

struct omemo_context *omemo_context_create(const signal_protocol_address *own_address);

void omemo_context_free(struct omemo_context *context);


#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_CONTEXT_H */

#ifndef PROF_OMEMO_CONTEXT_H
#define PROF_OMEMO_CONTEXT_H

#include <pthread.h>
#include <signal_protocol_types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omemo_context {
	signal_protocol_address own_address;
	signal_context *signal_ctx;
	signal_protocol_store_context *signal_store_ctx;
	
	struct omemo_store_context *store_context;
	struct omemo_bundle *bundle;
	
	pthread_mutex_t mutex;
};

struct omemo_context *omemo_context_create(const signal_protocol_address *address);

void omemo_context_free(struct omemo_context *context);



#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_CONTEXT_H */

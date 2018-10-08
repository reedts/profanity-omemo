#ifndef PROF_OMEMO_CONTEXT_DEF_H
#define PROF_OMEMO_CONTEXT_DEF_H

struct omemo_context {
	signal_protocol_address own_address;
	signal_context *signal_ctx;
	signal_protocol_store_context *signal_store_ctx;

	struct omemo_store_context *store_context;
	struct omemo_bundle *bundle;

	pthread_mutex_t mutex;
};

#endif /* PROF_OMEMO_CONTEXT_DEF_H */

#ifndef PROF_OMEMO_CONTEXT_H
#define PROF_OMEMO_CONTEXT_H

#include <pthread.h>
#include <signal_protocol_types.h>
#include <omemo/omemo_types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omemo_context;

struct omemo_context *omemo_context_create(const signal_protocol_address *own_address);

int omemo_context_install(struct omemo_context *ctx);

void omemo_context_free(struct omemo_context *context);


#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_CONTEXT_H */

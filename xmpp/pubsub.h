#ifndef PROF_OMEMO_PUBSUB_H
#define PROF_OMEMO_PUBSUB_H

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device_list;

int omemo_publish_device_list(const char *jid, struct device_list **list);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_PUBSUB_H */

//
// Created by reedts 2/24/18
//

#ifndef PROF_OMEMO_PUBSUB_H
#define PROF_OMEMO_PUBSUB_H

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device_list;

/**
 * Generates a stanza for device list publishing.
 * Memory for the returned string is allocated using `malloc()`. The caller
 * is responsible for freeing this buffer.
 * @param jid JID of the user whose device list stanza is generated.
 * @param list The device list a publish stanza shall be created for.
 * @return Ready to send stanza to publish device list or `NULL` on error
 *	   in which case `errno` is set to indicate the source.
 */
char *omemo_publish_device_list_stanza(const char *jid, const struct device_list *list);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_PUBSUB_H */

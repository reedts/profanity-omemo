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
char *omemo_generate_device_list_stanza(const char *jid, const struct device_list *list);

/**
 * Generates an OMEMO device list from the received stanza.
 * Memory for the returned device list is allocated using `malloc()`. The caller
 * should free the device list with `omemo_device_list_free()`.
 * @param stanza Stanza to process to device list.
 * @return Deserialised device list or `NULL` on error condition in which case `errno`
 *		is set to indicate the source.
 */
struct device_list *omemo_process_device_list_stanza(const char *stanza);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_PUBSUB_H */

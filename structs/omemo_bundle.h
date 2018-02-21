//
// Created by reedts on 2/21/18.
//

#ifndef PROF_OMEMO_BUNDLE_H
#define PROF_OMEMO_BUNDLE_H

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omemo_context;

typedef signal_protocol_key_helper_pre_key_list_node pre_key_list;

struct omemo_bundle {
	ec_public_key *signed_pre_key;
	uint32_t signed_pre_key_id;
	signal_buffer *signed_pre_key_signature;
	ec_public_key *pub_key;
	pre_key_list *prekeys;
};

/**
 * Create new OMEMO bundle.
 * @param context The OMEMO user context, a bundle should be created for.
 * @return Pointer to an initialised OMEMO bundle or `NULL` on error in which case `errno` is set
 *	   to indicate the cause.
 */
struct omemo_bundle *omemo_bundle_create(struct omemo_context *context);

/**
 * Serializes an OMEMO bundle into XML.
 * The XML output is ready to be embedded into a stanza.
 * @param root XML root to start serialization at.
 * @param bundle OMEMO bundle to serialize.
 * @retval 0 Bundle was successfully serialized.
 * @retval -1 An error occurred and `errno` is set to indicate the cause.
 */
int omemo_bundle_serialize_xml(xmlNodePtr *root, const struct omemo_bundle *bundle);

/**
 * Constructs an OMEMO bundle from XML.
 * @param bundle Handle that is initialised with the deserialized OMEMO bundle.
 * @param node XML node to start deserialization from.
 * @param address Address of the contact the OMEMO bundle belongs to.
 * @retval 0 Bundle was successfully deserialized.
 * @retval -1 An error occurred and `errno` is set to indicate the cause.
 */
int omemo_bundle_deserialize_xml(struct omemo_bundle *bundle, xmlNodePtr node, const char *barejid);

/**
 * Frees an OMEMO bundle.
 * @param bundle OMEMO bundle to free.
 */
void omemo_bundle_free(struct omemo_bundle *bundle);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_BUNDLE_H */

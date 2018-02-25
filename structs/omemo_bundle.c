#if _POSIX_C_SOURCE <= 200809L || !defined( _POSIX_C_SOURCE )
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <key_helper.h>
#include <omemo/omemo_constants.h>
#include <signal_protocol.h>
#include <stdlib.h>
#include <store/omemo_store.h>
#include <string.h>
#include <structs/omemo_context.h>
#include <structs/omemo_bundle.h>

#define UNUSED(x) (void)(x)

#define OMEMO_BUNDLE_NODE_NAME "bundle"
#define OMEMO_BUNDLE_SIGNED_PRE_KEY_NODE_NAME "signedPreKeyPublic"
#define OMEMO_BUNDLE_SIGNED_PRE_KEY_SIGNATURE_NODE_NAME "signedPreKeySignature"
#define OMEMO_BUNDLE_IDENTITY_KEY_NODE_NAME "identityKey"
#define OMEMO_BUNDLE_PRE_KEYS_NODE_NAME "prekeys"
#define OMEMO_BUNDLE_PRE_KEY_NAME "preKeyPublic"


struct omemo_bundle *omemo_bundle_create(struct omemo_context *context)
{
	int retval;
	signal_buffer *pub_key_buf = NULL;
	signal_buffer *priv_key_buf = NULL;
	signal_buffer *sig_pre_key_buf = NULL;
	signal_buffer *pre_key_buf = NULL;
	session_signed_pre_key *sig_pre_key = NULL;

	struct omemo_bundle *bundle;

	bundle = malloc(sizeof(struct omemo_bundle));
	if (!bundle) {
		errno = ENOMEM;
		return NULL;
	}

	/* Load public key of identity key pair */
	retval = context->store_context->identity_key_store.get_identity_key_pair(&pub_key_buf, &priv_key_buf, context);
	if (retval < 0) {
		return NULL;
	}

	curve_decode_point(&bundle->pub_key, signal_buffer_data(pub_key_buf), signal_buffer_len(pub_key_buf), context->signal_ctx);
	signal_buffer_free(pub_key_buf);
	signal_buffer_bzero_free(priv_key_buf);

	/* Load signed pre key */
	/* TODO: Value of signed_pre_key id?? */
	bundle->signed_pre_key_id = 0;
	retval = context->store_context->signed_pre_key_store.load_signed_pre_key(&sig_pre_key_buf, bundle->signed_pre_key_id, context);
	if (retval < 0) {
		return NULL;
	}

	/* Deserialise signed pre key pair from buffer */
	session_signed_pre_key_deserialize(&sig_pre_key, signal_buffer_data(sig_pre_key_buf),
	                                   signal_buffer_len(sig_pre_key_buf), context->signal_ctx);
	signal_buffer_free(sig_pre_key_buf);
	sig_pre_key_buf = NULL;

	/* Obtain signed public pre key */
	ec_public_key_serialize(&sig_pre_key_buf,
	                        ec_key_pair_get_public(session_signed_pre_key_get_key_pair(sig_pre_key)));

	curve_decode_point(&bundle->signed_pre_key, signal_buffer_data(sig_pre_key_buf), signal_buffer_len(sig_pre_key_buf),
	                   context->signal_ctx);
	signal_buffer_free(sig_pre_key_buf);

	/* Set signed pre key signature */
	bundle->signed_pre_key_signature = signal_buffer_create(session_signed_pre_key_get_signature(sig_pre_key),
	                                   session_signed_pre_key_get_signature_len(sig_pre_key));
	if (!bundle->signed_pre_key_signature) {
		return NULL;
	}

	/* TODO: Load pre keys */

	return bundle;
}

int omemo_bundle_serialize_xml(xmlNodePtr *root, const struct omemo_bundle *bundle)
{
	char cur_pre_key_id_str[32];
	char *cur_pre_key_str;
	pre_key_list *pre_key_it;
	session_pre_key *cur_pre_key;
	signal_buffer *cur_pre_key_buf;
	uint32_t cur_pre_key_id;
	xmlNodePtr bundle_root;
	xmlNodePtr pre_key_node;
	xmlNodePtr sig_pre_key_node;
	xmlNsPtr bundle_ns;

	if (!root || !bundle) {
		errno = EINVAL;
		return -1;
	}

	UNUSED(bundle_ns);

	bundle_root = xmlNewNode(NULL, BAD_CAST OMEMO_BUNDLE_NODE_NAME);
	bundle_ns = xmlNewNs(bundle_root, BAD_CAST OMEMO_XML_NS, NULL);

	/* TODO: Use bundle's real signed pre key */
	sig_pre_key_node = xmlNewChild(bundle_root, NULL, BAD_CAST OMEMO_BUNDLE_SIGNED_PRE_KEY_NODE_NAME,
	                               BAD_CAST "SOMEBASE64ENCODEDSTRING");
	/* TODO: Use bundle's real signed pre key id */
	xmlNewProp(sig_pre_key_node, BAD_CAST "signedPreKeyId", BAD_CAST "1");

	/* TODO: Use bundle's real signed pre key signature */
	xmlNewChild(bundle_root, NULL, BAD_CAST OMEMO_BUNDLE_SIGNED_PRE_KEY_SIGNATURE_NODE_NAME,
	            BAD_CAST "SOMEBASE64ENCODEDSTRING");

	/* TODO: Use bundle's real public identity key */
	xmlNewChild(bundle_root, NULL, BAD_CAST OMEMO_BUNDLE_IDENTITY_KEY_NODE_NAME,
	            BAD_CAST "SOMEBASE64ENCODEDSTRING");

	/* Add prekey section */
	pre_key_node = xmlNewChild(bundle_root, NULL, BAD_CAST OMEMO_BUNDLE_PRE_KEYS_NODE_NAME, NULL);

	/* TODO: Add all prekeys here */
	for (pre_key_it = bundle->prekeys; pre_key_it; pre_key_it = signal_protocol_key_helper_key_list_next(pre_key_it)) {
		cur_pre_key = signal_protocol_key_helper_key_list_element(pre_key_it);
		session_pre_key_serialize(&cur_pre_key_buf, cur_pre_key);
		cur_pre_key_id = session_pre_key_get_id(cur_pre_key);
		snprintf(cur_pre_key_id_str, 32, "%u", cur_pre_key_id);
		cur_pre_key_str = strndup((char *)signal_buffer_data(cur_pre_key_buf), signal_buffer_len(cur_pre_key_buf));
		if (!cur_pre_key_str) {
			return -1;
		}

		xmlNodePtr key_elem = xmlNewChild(pre_key_node, NULL, BAD_CAST "preKeyPublic",
		                                  BAD_CAST cur_pre_key_str);
		xmlNewProp(key_elem, BAD_CAST "preKeyId", BAD_CAST cur_pre_key_id_str);
		signal_buffer_free(cur_pre_key_buf);
		free(cur_pre_key_str);
	}

	xmlAddChild(*root, bundle_root);

	return 0;
}

int omemo_bundle_deserialize_xml(struct omemo_bundle *bundle, xmlNodePtr node, const char *barejid)
{
	/* Maybe use signal_protocol_address inplace of simple string */

	/* TODO: Actual deserialization */
}

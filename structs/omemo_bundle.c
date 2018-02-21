#include <errno.h>
#include <signal_protocol.h>
#include <stdlib.h>
#include <store/omemo_store.h>
#include <structs/omemo_context.h>
#include <structs/omemo_bundle.h>

struct omemo_bundle *omemo_bundle_create_own(struct omemo_context *context)
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

	return bundle;
}

int omemo_bundle_serialize_xml(xmlNodePtr *root, const struct omemo_bundle *bundle)
{
	/* TODO: Actual serialization */
}

int omemo_bundle_deserialize_xml(struct omemo_bundle *bundle, xmlNodePtr node, const char *barejid)
{
	/* Maybe use signal_protocol_address inplace of simple string */

	/* TODO: Actual deserialization */
}

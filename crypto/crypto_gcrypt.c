#include <gcrypt.h>
#include <signal/signal_protocol.h>

#include "crypto_provider.h"

int omemo_init_provider()
{
	if (!gcry_check_version(NULL)) {
		fputs("Wrong libgcrypt version!", stderr);
		return -1;
	}

	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

	return 0;
}

int omemo_random_numbers(uint8_t *data, size_t len, void *user_data)
{
	if (!data) {
		return SG_ERR_INVAL;
	}

	gcry_randomize(data, len, GCRY_STRONG_RANDOM);

	return 0;
}

int omemo_hmac_sha256_init(void **hmac_context, const uint8_t *key,
			   size_t data_len, void *user_data)
{
	if (!hmac_context || !key) {
		return SG_ERR_INVAL;
	}

	UNUSED(user_data);
	
	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_mac_hd_t *hd = NULL;

	hd = malloc(sizeof(gcry_mac_hd_t));
	if (!hd) {
		return SG_ERR_NOMEM;
	}

	err = gcry_mac_open(hd, GCRY_MAC_HMAC_SHA256, 0, NULL);
	if (err) {
		free(hd);
		return SG_ERR_UNKNOWN;
	}

	err = gcry_mac_setkey(*hd, key, data_len);
	if (err) {
		free(hd);
		return SG_ERR_UNKNOWN;
	}

	*hmac_context = hd;

	return 0;
}

int omemo_hmac_sha256_update(void *hmac_context, const uint8_t *data,
			     size_t data_len, void *user_data)
{
	if (!hmac_context || !data) {
		return SG_ERR_INVAL;
	}

	UNUSED(user_data);

	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_mac_hd_t *hd = hmac_context;

	err = gcry_mac_write(*hd, data, data_len);
	if (err) {
		free(hd);
		return SG_ERR_UNKNOWN;
	}

	return 0;
}

int omemo_hmac_sha256_final(void *hmac_context, signal_buffer **output,
			     void *user_data)
{
	UNUSED(user_data);

	if (!hmac_context || !output) {
		return SG_ERR_INVAL;
	}

	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_mac_hd_t *hd = hmac_context;
	signal_buffer *sig_buffer = NULL;
	size_t mac_len;
	
	mac_len = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA256);
	uint8_t buffer[mac_len];

	err = gcry_mac_read(*hd, buffer, &mac_len);
	if (err) {
		free(hd);
		return SG_ERR_UNKNOWN;
	}

	sig_buffer = signal_buffer_create(buffer, mac_len);
	if (!sig_buffer) {
		free(hd);
		return SG_ERR_UNKNOWN;
	}

	*output = sig_buffer;

	return 0;
}

void omemo_hmac_sha256_cleanup(void *hmac_context, void *user_data)
{
	UNUSED(user_data);

	gcry_mac_hd_t *hd = hmac_context;

	if (!hmac_context) {
		return;
	}
	
	gcry_mac_close(*hd);
	free(hmac_context);
}

int omemo_sha512_digest_init(void **digest_context, void *user_data)
{
	UNUSED(user_data);
	
	if (!digest_context) {
		return SG_ERR_INVAL;
	}

	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_md_hd_t *hd = NULL;

	hd = malloc(sizeof(gcry_md_hd_t));
	if (!hd) {
		return SG_ERR_NOMEM;
	}

	err = gcry_md_open(hd, GCRY_MD_SHA512, 0);
	if (err < 0) {
		return SG_ERR_UNKNOWN;
	}

	*digest_context = hd;

	return 0;
}

int omemo_sha512_digest_update(void *digest_context, const uint8_t *data,
			       size_t data_len, void *user_data)
{
	UNUSED(user_data);

	if (!digest_context || !data) {
		puts("digest update error");
		return SG_ERR_INVAL;
	}
	
	gcry_md_hd_t *hd = digest_context;

	gcry_md_write(*hd, data, data_len);

	return 0;
}

int omemo_sha512_digest_final(void *digest_context, signal_buffer **output,
			      void *user_data)
{
	UNUSED(user_data);

	if (!digest_context || !output) {
		return SG_ERR_INVAL;
	}
	
	uint8_t *buffer = NULL;
	gcry_md_hd_t *hd = digest_context;
	signal_buffer *out = NULL;
	size_t len;

	len = gcry_md_get_algo_dlen(GCRY_MD_SHA512);

	buffer = gcry_md_read(*hd, GCRY_MD_SHA512);
	if (!buffer) {
		puts("digest final error");
		return SG_ERR_UNKNOWN;
	}

	out = signal_buffer_create(buffer, len);
	if (!out) {
		puts("digest final error");
		return SG_ERR_NOMEM;
	}
	
	*output = out;

	return 0;
}

void omemo_sha512_digest_cleanup(void *digest_context, void *user_data)
{
	UNUSED(user_data);

	if (!digest_context) {
		puts("digest cleanup error");
		return;
	}
	
	gcry_md_hd_t *hd = digest_context;

	gcry_md_close(*hd);

	//free(digest_context);
}


int omemo_encrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *plaintext, size_t plaintext_len,
		  void *user_data)
{ }

int omemo_decrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *ciphertext, size_t ciphertext_len,
		  void *user_data)
{ }


signal_crypto_provider omemo_crypto_provider = {
	.random_func			= omemo_random_numbers,
	.hmac_sha256_init_func		= omemo_hmac_sha256_init,
	.hmac_sha256_update_func	= omemo_hmac_sha256_update,
	.hmac_sha256_final_func		= omemo_hmac_sha256_final,
	.hmac_sha256_cleanup_func	= omemo_hmac_sha256_cleanup,
	.sha512_digest_init_func	= omemo_sha512_digest_init,
	.sha512_digest_update_func	= omemo_sha512_digest_update,
	.sha512_digest_final_func	= omemo_sha512_digest_final,
	.sha512_digest_cleanup_func	= omemo_sha512_digest_cleanup
};

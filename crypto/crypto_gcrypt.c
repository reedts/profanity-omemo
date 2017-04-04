#include <gcrypt.h>
#include <signal_protocol.h>

#include "crypto_provider.h"

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
	if (!hmac_context) {
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

	*handle = hd;

	return 0;
}

int omemo_hmac_sha256_update(void *hmac_context, const uint8_t *data,
			     size_t data_len, void *user_data)
{
	if (!hmac) {
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

	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_mac_hd_t *hd = hmac_context;
	signal_buffer *sig_buffer = NULL;
	size_t mac_len;
	
	mac_len = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA256);
	uint8_t buffer[mac_len];

	memset(buffer, 0x0, mac_len);

	err = gcry_mac_read(*hd, buffer, mac_len);

	sig_buffer = signal_buffer_create(buffer, mac_len);
	if (!sig_buffer) {
		return SG_ERR_UNKNOWN;
	}

	*output = sig_buffer;

	return 0;
}

void omemo_hmac_sha256_cleanup(void *hmac_context, void *user_data)
{
	UNUSED(user_data);
	
	gcry_mac_close(*hmac_context);
	free(hmac_context);
}

int omemo_sha512_digest_init(void **digest_context, void *user_data)
{
	UNUSED(user_data);

	gcry_error_t err = GPG_ERR_NO_ERROR;
	gcry_md_hd_t *hd = NULL;

	hd = malloc(sizeof(gcry_md_hd_t));
	if (!hd) {
		return SG_ERR_NOMEM;
	}

	err = gcry_md_open(gcry_md_hd_t, GCRY_MD_SHA512, 0);

	*digest_context = hd;

	return 0;
}
	

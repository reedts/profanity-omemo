#include <gcrypt.h>
#include <signal_protocol.h>

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
	free(digest_context);
}

static int omemo_endecrypt_aes_ctr(signal_buffer **output, const uint8_t *key,
				   size_t key_len, const uint8_t *iv,
				   size_t iv_len, const uint8_t *text,
				   size_t text_len, int mode)
{
	int cipher_mode = GCRY_CIPHER_MODE_CTR;
	int cipher_algo;
	gcry_cipher_hd_t handle;
	uint8_t *buffer;
	gcry_error_t err = GPG_ERR_NO_ERROR;
	signal_buffer *output_buffer;
	
	switch (key_len) {
	case 16:
		cipher_algo = GCRY_CIPHER_AES128;
		break;
	case 24:
		cipher_algo = GCRY_CIPHER_AES192;
		break;
	case 32:
		cipher_algo = GCRY_CIPHER_AES256;
		break;
	default:
		puts("unknown cipher length");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_open(&handle, cipher_algo, cipher_mode, 0);
	if (err) {
		puts("cipher open error");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setkey(handle, key, key_len);
	if (err) {
		puts("cipher set key error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setiv(handle, iv, iv_len);
	if (err) {
		puts("cipher set iv error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	buffer = malloc(text_len);
	if (!buffer) {
		gcry_cipher_close(handle);
		return SG_ERR_NOMEM;
	}

	if (mode == AES_CTR_ENCRYPT) {
		err = gcry_cipher_encrypt(handle, buffer, text_len, text, text_len);
	} else {
		err = gcry_cipher_decrypt(handle, buffer, text_len, text, text_len);
	}
	if (err) {
		puts("encrypt/decrypt error");
		gcry_cipher_close(handle);
		free(buffer);
		return SG_ERR_UNKNOWN;
	}

	output_buffer = signal_buffer_create(buffer, text_len);
	if (!output_buffer) {
		gcry_cipher_close(handle);
		free(buffer);
		return SG_ERR_NOMEM;
	}

	gcry_cipher_close(handle);
	free(buffer);

	*output = output_buffer;

	return 0;
}

static int omemo_encrypt_aes_cbc(signal_buffer **output,
				 const uint8_t *key, size_t key_len,
				 const uint8_t *iv, size_t iv_len,
				 const uint8_t *plaintext, size_t plaintext_len)
{
	int cipher_mode = GCRY_CIPHER_MODE_CBC;
	int cipher_algo;
	gcry_cipher_hd_t handle;
	gcry_error_t err = GPG_ERR_NO_ERROR;
	uint8_t *buffer;
	uint8_t *plaintext_padded;
	size_t block_size;
	size_t padding_len;
	size_t plaintext_padded_len;
	signal_buffer *output_buffer;

	switch (key_len) {
	case 16:
		cipher_algo = GCRY_CIPHER_AES128;
		break;
	case 32:
		cipher_algo = GCRY_CIPHER_AES192;
		break;
	case 64:
		cipher_algo = GCRY_CIPHER_AES256;
		break;
	default:
		puts("unknown cipher length");
		return SG_ERR_UNKNOWN;
	}

	block_size = gcry_cipher_get_algo_blklen(cipher_algo);
	if (!block_size) {
		puts("cipher block length error");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_open(&handle, cipher_algo, cipher_mode, 0);
	if (err) {
		puts("cipher open error");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setkey(handle, key, key_len);
	if (err) {
		puts("cipher set key error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setiv(handle, iv, iv_len);
	if (err) {
		puts("cipher set iv error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	padding_len = block_size - (plaintext_len % block_size);
	plaintext_padded_len = plaintext_len + padding_len;
	plaintext_padded = malloc(plaintext_padded_len);
	if (!plaintext_padded) {
		return SG_ERR_NOMEM;
	}

	memset(plaintext_padded, (uint8_t) padding_len, plaintext_padded_len);
	memcpy(plaintext_padded, plaintext, plaintext_len);
	
	buffer = malloc(plaintext_padded_len);
	if (!buffer) {
		free(plaintext_padded);
		gcry_cipher_close(handle);
		return SG_ERR_NOMEM;
	}

	err = gcry_cipher_encrypt(handle, buffer, plaintext_padded_len, plaintext_padded,
				  plaintext_padded_len);
	if (err) {
		puts("cipher encrypt error");
		gcry_cipher_close(handle);
		free(plaintext_padded);
		free(buffer);
		return SG_ERR_UNKNOWN;
	}
	
	output_buffer = signal_buffer_create(buffer, plaintext_padded_len);
	if (!output_buffer) {
		gcry_cipher_close(handle);
		free(plaintext_padded);
		free(buffer);
		return SG_ERR_NOMEM;
	}

	*output = output_buffer;
	
	gcry_cipher_close(handle);
	free(plaintext_padded);
	free(buffer);

	return 0;
}

int omemo_encrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *plaintext, size_t plaintext_len,
		  void *user_data)
{
	UNUSED(user_data);
	
	if (!output || !key || !iv || !plaintext) {
		return SG_ERR_UNKNOWN;
	}

	if (cipher == SG_CIPHER_AES_CTR_NOPADDING) {
		return omemo_endecrypt_aes_ctr(output, key, key_len, iv, iv_len, plaintext, plaintext_len, AES_CTR_ENCRYPT);
	} else if (cipher == SG_CIPHER_AES_CBC_PKCS5) {
		return omemo_encrypt_aes_cbc(output, key, key_len, iv, iv_len, plaintext, plaintext_len);
	} else {
		return SG_ERR_UNKNOWN;
	}
}

static int omemo_decrypt_aes_cbc(signal_buffer **output, const uint8_t *key,
				 size_t key_len, const uint8_t *iv,
				 size_t iv_len, const uint8_t *ciphertext,
				 size_t ciphertext_len)
{
	int cipher_mode = GCRY_CIPHER_MODE_CBC;
	int cipher_algo;
	gcry_cipher_hd_t handle;
	gcry_error_t err = GPG_ERR_NO_ERROR;
	uint8_t *buffer;
	signal_buffer *output_buffer;
	size_t padding_len;
	size_t plaintext_unpadded_len;

	switch (key_len) {
	case 16:
		cipher_algo = GCRY_CIPHER_AES128;
		break;
	case 32:
		cipher_algo = GCRY_CIPHER_AES192;
		break;
	case 64:
		cipher_algo = GCRY_CIPHER_AES256;
		break;
	default:
		puts("unknown cipher length");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_open(&handle, cipher_algo, cipher_mode, 0);
	if (err) {
		puts("cipher open error");
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setkey(handle, key, key_len);
	if (err) {
		puts("cipher set key error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	err = gcry_cipher_setiv(handle, iv, iv_len);
	if (err) {
		puts("cipher set iv error");
		gcry_cipher_close(handle);
		return SG_ERR_UNKNOWN;
	}

	buffer = malloc(ciphertext_len);
	if (!buffer) {
		gcry_cipher_close(handle);
		return SG_ERR_NOMEM;
	}

	err = gcry_cipher_decrypt(handle, buffer, ciphertext_len, ciphertext, ciphertext_len);
	if (err) {
		puts("cipher encrypt error");
		gcry_cipher_close(handle);
		free(buffer);
		return SG_ERR_UNKNOWN;
	}

	padding_len = buffer[ciphertext_len-1];
	plaintext_unpadded_len = ciphertext_len - padding_len;

	output_buffer = signal_buffer_create(buffer, plaintext_unpadded_len);
	if (!output_buffer) {
		gcry_cipher_close(handle);
		free(buffer);
		return SG_ERR_NOMEM;
	}

	*output = output_buffer;
	
	gcry_cipher_close(handle);
	free(buffer);

	return 0;
}

int omemo_decrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *ciphertext, size_t ciphertext_len,
		  void *user_data)
{
	UNUSED(user_data);

	if (!output || !key || !iv || !ciphertext) {
		return SG_ERR_UNKNOWN;
	}

	if (cipher == SG_CIPHER_AES_CTR_NOPADDING) {
		return omemo_endecrypt_aes_ctr(output, key, key_len, iv, iv_len, ciphertext, ciphertext_len, AES_CTR_DECRYPT);
	} else if (cipher == SG_CIPHER_AES_CBC_PKCS5) {
		return omemo_decrypt_aes_cbc(output, key, key_len, iv, iv_len, ciphertext, ciphertext_len);
	} else {
		return SG_ERR_UNKNOWN;
	}
}


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

#ifndef PROF_OMEMO_CRYPTO_PROVIDER_H
#define PROF_OMEMO_CRYPTO_PROVIDER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED(x) (void)(x)

extern signal_crypto_provider omemo_crypto_provider;

int omemo_init_provider(void);

int omemo_random_numbers(uint8_t *data, size_t len, void *user_data);

int omemo_hmac_sha256_init(void **hmac_context, const uint8_t *key,
			   size_t key_len, void *user_data);

int omemo_hmac_sha256_update(void *hmac_context, const uint8_t *data,
			     size_t data_len, void *user_data);

int omemo_hmac_sha256_final(void *hmac_context, signal_buffer **output,
			    void *user_data);

void omemo_hmac_sha256_cleanup(void *hmac_context, void *user_data);

int omemo_sha512_digest_init(void **digest_context, void *user_data);

int omemo_sha512_digest_update(void *digest_context, const uint8_t *data,
			       size_t data_len, void *user_data);

int omemo_sha512_digest_final(void *digest_context, signal_buffer **output,
			      void *user_data);

void omemo_sha512_digest_cleanup(void *digest_context, void *user_data);


int omemo_encrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *plaintext, size_t plaintext_len,
		  void *user_data);

int omemo_decrypt(signal_buffer **output, int cipher,
		  const uint8_t *key, size_t key_len,
		  const uint8_t *iv, size_t iv_len,
		  const uint8_t *ciphertext, size_t ciphertext_len,
		  void *user_data);


#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_CRYPTO_PROVIDER_H */

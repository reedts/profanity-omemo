#include "test_main.h"

TEST(crypto, hmac256)
{
	const char *test = "Hello, OMEMO!";
	const char *key = "key";
	
	gcry_mac_hd_t res_ctx;
	uint8_t *output = NULL;
	signal_buffer *buffer = NULL;
	size_t hash_len;
	void *context = NULL;

	omemo_hmac_sha256_init(&context, (uint8_t *)key, strlen(key), NULL);
	omemo_hmac_sha256_update(context, (uint8_t *)test, strlen(test), NULL);
	omemo_hmac_sha256_final(context, &buffer, NULL);
	omemo_hmac_sha256_cleanup(context, NULL);
	
	hash_len = gcry_mac_test_algo(GCRY_MAC_HMAC_SHA256);
	uint8_t result[hash_len];
	memset(result, 0x0, hash_len);
	
	output = signal_buffer_data(buffer);

	gcry_mac_open(&res_ctx, GCRY_MAC_HMAC_SHA256, 0, NULL);
	gcry_mac_setkey(res_ctx, key, strlen(key));
	gcry_mac_write(res_ctx, test, strlen(test));
	gcry_mac_read(res_ctx, result, &hash_len);
	gcry_mac_close(res_ctx);

	ASSERT_EQ(memcmp(result, output, hash_len), 0);

	signal_buffer_free(buffer);
}

TEST(crypto, sha512)
{
	const char *test = "Hello, OMEMO!";

	uint8_t *output = NULL;
	uint8_t result[64];
	signal_buffer *buffer = NULL;;	
	size_t buffer_len;
	void *context = NULL;

	omemo_init_provider();

	omemo_sha512_digest_init(&context, NULL);
	omemo_sha512_digest_update(context, (uint8_t *)test, strlen(test), NULL);
	omemo_sha512_digest_final(context, &buffer, NULL);
	omemo_sha512_digest_cleanup(context, NULL);

	memset(result, 0x0, 64);
	
	output = signal_buffer_data(buffer);

	gcry_md_hash_buffer(GCRY_MD_SHA512, result, test, strlen(test));
	
	ASSERT_EQ(memcmp(result, output, signal_buffer_len(buffer)), 0);

	signal_buffer_free(buffer);
}


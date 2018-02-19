#include <gtest.h>
#include <gcrypt.h>
#include <signal_protocol.h>
#include <sys/stat.h>

#include <string>
#include <fstream>

#include <crypto/crypto_provider.h>
#include <store/omemo_store.h>
#include <structs/device_list.h>
#include <structs/omemo_device.h>
#include <xmpp/pubsub.h>
#include <xmpp/omemo_constants.h>

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


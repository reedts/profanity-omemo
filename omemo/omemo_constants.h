#ifndef PROF_OMEMO_OMEMO_CONST_H
#define PROF_OMEMO_OMEMO_CONST_H

#include <stdint.h>

static const char PUBSUB_PROTO_XML_NS[] = "http://jabber.org/protocol/pubsub";

static const char OMEMO_WORKING_DIR[] = ".local/share/profanity/omemo";

static const char OMEMO_DEVICE_LIST_PUBLISH_XML_NODE[] = "urn:xmpp:omemo:0:devicelist";
static const char OMEMO_DEVICE_LIST_XML_NS[] = "urn:xmpp:omemo:0";

static const uint32_t OMEMO_NUM_PRE_KEYS = 100;

#endif /* PROF_OMEMO_OMEMO_CONST_H */

#if _POSIX_C_SOURCE <= 200809L || !defined( _POSIX_C_SOURCE )
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <omemo/omemo_constants.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <structs/device_list.h>
#include <xmpp/pubsub.h>

char *omemo_generate_device_list_stanza(const char *jid, const struct device_list *list)
{
	char *stanza;
	int stanza_size;
	xmlBufferPtr buf = NULL;
	xmlNodePtr root, pubsub, publish, item;
	xmlNsPtr pubsub_ns;

	pubsub_ns = xmlNewNs(NULL, BAD_CAST PUBSUB_PROTO_XML_NS, NULL);
	if (!pubsub_ns) {
		errno = EINVAL;
		return NULL;
	}

	root = xmlNewNode(NULL, BAD_CAST "iq");
	xmlNewProp(root, BAD_CAST "from", BAD_CAST jid        );
	xmlNewProp(root, BAD_CAST "type", BAD_CAST "set"      );
	xmlNewProp(root, BAD_CAST "id",   BAD_CAST "announce1");

	pubsub = xmlNewChild(root, pubsub_ns, BAD_CAST "pubsub", NULL);
	publish = xmlNewChild(pubsub, NULL, BAD_CAST "publish", NULL);
	xmlNewProp(publish, BAD_CAST "node", BAD_CAST OMEMO_DEVICE_LIST_PUBLISH_XML_NODE);

	item = xmlNewChild(publish, NULL, BAD_CAST "item", NULL);

	if (omemo_device_list_serialize_xml(&item, list) < 0) {
		goto err_return;
	}

	/* Now Dump everything in the buffer and copy it into string to send */
	buf = xmlBufferCreate();
	stanza_size = xmlNodeDump(buf, NULL, root, 0, 1);
	if (stanza_size < 0) {
		errno = EIO;
		goto err_return;
	}

	stanza = strndup((char *) xmlBufferContent(buf), stanza_size);

	xmlBufferFree(buf);
	xmlFreeNodeList(root);

	xmlCleanupParser();

	return stanza;

err_return:
	if (!buf) xmlBufferFree(buf);
	xmlFreeNodeList(root);
	xmlCleanupParser();

	return NULL;
}



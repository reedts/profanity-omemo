#include <errno.h>
#include <omemo/omemo_constants.h>
#include <stdio.h>
#include <stdlib.h>
#include <structs/device_list.h>
#include <xmpp/pubsub.h>

int omemo_publish_device_list(const char *jid, struct device_list **list)
{
	int buf_size = 0;
	xmlBufferPtr buf = NULL;
	xmlNodePtr root, pubsub, publish, item;
	xmlNsPtr pubsub_ns;

	pubsub_ns = xmlNewNs(NULL, BAD_CAST PUBSUB_PROTO_XML_NS, NULL);
	if (!pubsub_ns) {
		errno = EINVAL;
		return -1;
	}

	root = xmlNewNode(NULL, BAD_CAST "iq");
	xmlNewProp(root, BAD_CAST "from", BAD_CAST jid        );
	xmlNewProp(root, BAD_CAST "type", BAD_CAST "set"      );
	xmlNewProp(root, BAD_CAST "id",   BAD_CAST "announce1");

	pubsub = xmlNewNode(pubsub_ns, BAD_CAST "pubsub");
	xmlAddChild(root, pubsub);

	publish = xmlNewNode(NULL, BAD_CAST "publish");
	xmlNewProp(publish, BAD_CAST "node", BAD_CAST OMEMO_DEVICE_LIST_PUBLISH_XML_NODE);
	xmlAddChild(pubsub, publish);

	item = xmlNewNode(NULL, BAD_CAST "item");
	xmlAddChild(publish, item);

	if (omemo_device_list_serialize_xml(&item, list) < 0) {
		return -1;
	}

	/* Now Dump everything in the buffer and copy it into string to send */
	buf = xmlBufferCreate();
	buf_size = xmlNodeDump(buf, NULL, root, 0, 1);
	if (buf_size < 0) {
		errno = EIO;
		return -1;
	}


	/* Send the stanza here */
	puts((char *)buf->content);

	xmlBufferFree(buf);
	xmlFreeNode(root);
	xmlFreeNs(pubsub_ns);

	xmlCleanupParser();

	return 0;
}



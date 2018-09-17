#if _POSIX_C_SOURCE <= 200809L || !defined( _POSIX_C_SOURCE )
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <omemo/omemo_constants.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <structs/device_list.h>
#include <structs/omemo_bundle.h>
#include <xmpp/pubsub.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

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
	xmlFreeNs(pubsub_ns);
	xmlCleanupParser();

	return stanza;

err_return:
	if (!buf) xmlBufferFree(buf);
	xmlFreeNodeList(root);
	xmlCleanupParser();

	return NULL;
}


struct device_list *omemo_process_device_list_stanza(const char *stanza)
{
	xmlDocPtr doc;
	xmlChar *jid;
	xmlNodePtr device_list_begin;
	xmlXPathContextPtr xpath_context;
	xmlXPathObjectPtr xpath_obj;

	struct device_list *list = NULL;

	if (!stanza) {
		errno = EINVAL;
		return NULL;
	}

	doc = xmlParseDoc(BAD_CAST stanza);
	if (!doc) {
		return NULL;
	}

	xpath_context = xmlXPathNewContext(doc);
	if (!xpath_context) {
		return NULL;
	}

	/* Select 'from' attribute of iq */
	xpath_obj = xmlXPathEvalExpression(BAD_CAST "/iq/@from", xpath_context);
	if (!xpath_obj) {
		errno = EINVAL;
		return NULL;
	}

	if (!xpath_obj->nodesetval) {
		errno = EINVAL;
		return NULL;
	}

	jid = (*xpath_obj->nodesetval->nodeTab)->children->content;
	xmlXPathFreeObject(xpath_obj);

	/* Select 'item' and all children */
	xpath_obj = xmlXPathEvalExpression(BAD_CAST "/iq/pubsub/publish/item/*", xpath_context);
	if (!xpath_obj) {
		errno = EINVAL;
		return NULL;
	}

	if (!xpath_obj->nodesetval) {
		errno = EINVAL;
		return NULL;
	}

	device_list_begin = *xpath_obj->nodesetval->nodeTab;

	if (omemo_device_list_deserialize_xml(&list, device_list_begin, (char *)jid) < 0) {
		return NULL;
	}

	xmlXPathFreeObject(xpath_obj);
	xmlXPathFreeContext(xpath_context);
	xmlFreeDoc(doc);

	return list;
}


char *omemo_generate_bundle_stanza(const char *jid, const struct omemo_bundle *bundle)
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
	xmlNewProp(root, BAD_CAST "id",   BAD_CAST "announce2");

	pubsub = xmlNewChild(root, pubsub_ns, BAD_CAST "pubsub", NULL);
	publish = xmlNewChild(pubsub, NULL, BAD_CAST "publish", NULL);
	xmlNewProp(publish, BAD_CAST "node", BAD_CAST OMEMO_BUNDLE_PUBLISH_XML_NODE);

	item = xmlNewChild(publish, NULL, BAD_CAST "item", NULL);

	if (omemo_bundle_serialize_xml(&item, bundle) < 0) {
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
	xmlFreeNs(pubsub_ns);
	xmlCleanupParser();

	return stanza;

err_return:
	if (!buf) xmlBufferFree(buf);
	xmlFreeNodeList(root);
	xmlCleanupParser();

	return NULL;
}


struct omemo_bundle *omemo_process_bundle_stanza(const char *stanza)
{
	xmlDocPtr doc;
	xmlChar *jid;
	xmlNodePtr bundle_begin;
	xmlXPathContextPtr xpath_context;
	xmlXPathObjectPtr xpath_obj;

	struct omemo_bundle *bundle = NULL;

	if (!stanza) {
		errno = EINVAL;
		return NULL;
	}

	doc = xmlParseDoc(BAD_CAST stanza);
	if (!doc) {
		return NULL;
	}

	xpath_context = xmlXPathNewContext(doc);
	if (!xpath_context) {
		return NULL;
	}

	/* Select 'from' attribute of iq */
	xpath_obj = xmlXPathEvalExpression(BAD_CAST "/iq/@from", xpath_context);
	if (!xpath_obj) {
		errno = EINVAL;
		return NULL;
	}

	if (!xpath_obj->nodesetval) {
		errno = EINVAL;
		return NULL;
	}

	jid = (*xpath_obj->nodesetval->nodeTab)->children->content;
	xmlXPathFreeObject(xpath_obj);

	/* Select 'item' and all children */
	xpath_obj = xmlXPathEvalExpression(BAD_CAST "/iq/pubsub/publish/item/*", xpath_context);
	if (!xpath_obj) {
		errno = EINVAL;
		return NULL;
	}

	if (!xpath_obj->nodesetval) {
		errno = EINVAL;
		return NULL;
	}

	bundle_begin = *xpath_obj->nodesetval->nodeTab;

	if (omemo_bundle_deserialize_xml(bundle, bundle_begin, (char *)jid) < 0) {
		return NULL;
	}

	xmlXPathFreeObject(xpath_obj);
	xmlXPathFreeContext(xpath_context);
	xmlFreeDoc(doc);

	return bundle;
}

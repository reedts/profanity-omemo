#if _POSIX_C_SOURCE <= 200809L || !defined( _POSIX_C_SOURCE )
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <structs/omemo_device.h>

struct omemo_device *omemo_device_create(const char *jid, int32_t id)
{
	if (!jid) {
		errno = EINVAL;
		return NULL;
	}

	struct omemo_device *new_device = malloc(sizeof(struct omemo_device));

	if (new_device) {
		size_t jid_length = strlen(jid);
		new_device->address.name = strndup(jid, jid_length);
		new_device->address.name_len = jid_length;
		new_device->address.device_id = id;

		new_device->trust = UNDECIDED;
		new_device->status = ACTIVE;
	}

	return new_device;
}

void omemo_device_free(struct omemo_device *device)
{
	free((char *)device->address.name);
	free(device);
}


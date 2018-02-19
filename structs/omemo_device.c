#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "omemo_device.h"

struct omemo_device *omemo_device_create(const char *jid, int32_t id)
{
	if (!jid) {
		errno = EINVAL;
		return NULL;
	}

	struct omemo_device *new_device = malloc(sizeof(struct omemo_device));

	if (new_device) {
		size_t jid_length = strlen(jid);
		new_device->address.name = jid;
		new_device->address.name_len = jid_length;
		new_device->address.device_id = id;

		new_device->trust = UNDECIDED;
		new_device->status = ACTIVE;
	}

	return new_device;
}

void omemo_device_free(struct omemo_device *device)
{
	free(device);
}


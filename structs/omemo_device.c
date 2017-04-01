#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "omemo_device.h"

struct omemo_device *omemo_device_create(const char *jid, uint32_t id)
{
	if (!jid) {
		errno = EINVAL;
		return NULL;
	}

	struct omemo_device *new_device = malloc(sizeof(struct omemo_device));

	if (new_device) {
		size_t jid_length = strlen(jid) + 1; //Null-Terminating
		new_device->jid = malloc(jid_length);
		
		strncpy(new_device->jid, jid, jid_length);

		new_device->id = id;
		new_device->trust = UNDECIDED;
		new_device->status = ACTIVE;
	}

	return new_device;
}

void omemo_device_free(struct omemo_device *device)
{
	free(device->jid);
	free(device);
}


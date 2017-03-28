#ifndef PROF_OMEMO_DEVICE_H
#define PROF_OMEMO_DEVICE_H

#include <stdint.h>

typedef enum _device_trust {
	UNDECIDED = 0,
	UNTRUSTED,
	TRUSTED,
	TRUST_COUNT
} device_trust;

typedef enum _device_status {
	INACTIVE = 0,
	ACTIVE,
	STATUS_COUNT
} device_status;

struct omemo_device {
	uint32_t id;
	char *jid;

	device_trust trust;
	device_status status;
	
	/* keys go here */

};

/**
 * @brief Allocates memory for a new OMEMO device.
 *
 * @param jid: JID the device belongs to.
 * @param id: ID the device will use.
 *
 * @return 'NULL' if an error occured, a pointer to the created OMEMO device
 *	   otherwise.
 */
struct omemo_device *omemo_device_create(const char *jid, uint32_t id);

/**
 * @brief Deallocates memory for an OMEMO device.
 *
 * @param device: Pointer to OMEMO device to be deleted.
 */
void omemo_device_free(struct omemo_device *device);


#endif /* PROF_OMEMO_DEVICE_H */

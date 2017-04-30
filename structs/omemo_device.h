#ifndef PROF_OMEMO_DEVICE_H
#define PROF_OMEMO_DEVICE_H

#include <stdint.h>

#include <signal_protocol_types.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	signal_protocol_address address;

	device_trust trust;
	device_status status;
	
	/* keys go here */

};

/**
 * @brief Allocates memory for a new OMEMO device.
 *
 * @param jid JID the device belongs to.
 * @param id ID the device will use.
 *
 * @return `NULL` if an error occured, a pointer to the created OMEMO device
 *	   otherwise.
 */
struct omemo_device *omemo_device_create(const char *jid, int32_t id);

/**
 * @brief Deallocates memory for an OMEMO device.
 *
 * @param device Pointer to OMEMO device to be deleted.
 */
void omemo_device_free(struct omemo_device *device);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_DEVICE_H */

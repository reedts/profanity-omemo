#ifndef PROF_OMEMO_DEVICE_LIST_H
#define PROF_OMEMO_DEVICE_LIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omemo_device;

struct device_list {
	struct device_list *next;
	struct omemo_device *device;
};

/**
 * @brief Creates a new empty device list.
 *
 * @param head Head of the list to be created
 *
 * @retval 0 The list was initialised successfully
 * @retval -1 An error occurred while initialising the list and
 *         `errno` was set to indicate the cause
 */
int omemo_device_list_create(struct device_list **head);

/**
 * @brief Adds an OMEMO device to the end of the device list.
 *
 * The list will free the `omeme_device` when it is not needed anymore.
 * Avoid freeing it manually or remove it from the list first.
 *
 * @param head Head of the list, the device shall be added to
 * @param device OMEMO device to add to the list
 *
 * @retval 1 The device was added to the list successfully
 * @retval 0 No errors occured but the device or an device with the same ID 
 *           is already in list.
 * @retval -1 An error occurred while adding the device to the list. `errno`
 *            is set to indicated the cause.
 */
int omemo_device_list_add(struct device_list **head,
			  struct omemo_device *device);

/**
 * @brief Adds an OMEMO device at the current position of the device list.
 *
 * The list will free the `omeme_device` when it is not needed anymore.
 * Avoid freeing it manually or remove it from the list first.
 *
 * @param head Head of the list to insert to
 * @param pos Position where the OMEMO device shall be inserted
 * @param device OMEMO device to add
 *
 * @retval 1 The device was added to the list successfully
 * @retval 0 No errors occured but the device or an device with the same ID 
 *           is already in list.
 * @retval -1 An error occurred while adding the device to the list. `errno`
 *            is set to indicated the cause.
 */
int omemo_device_list_add_inplace(struct device_list **head,
				  struct device_list *pos,
				  struct omemo_device *device);

/**
 * @brief Checks if the list contains a device.
 *
 * @param head Head of the list to look in
 * @param device device to look for.
 *
 * @retval 1 The list contains the device
 * @retval 0 The list does not contain the device
 * @retval -1 An error occurred and `errno` is set to indicate the cause
 */
int omemo_device_list_contains(struct device_list **head,
			       struct omemo_device *device);

/**
 * @brief Checks if the list contains a device with the ID.
 *
 * @param head Head of the list to look in
 * @param id device ID to look for.
 *
 * @retval 1 The list contains the device
 * @retval 0 The list does not contain the device
 * @retval -1 An error occurred and `errno` is set to indicate the cause
 */
int omemo_device_list_contains_id(struct device_list **head,
				  uint32_t id);

/**
 * @brief Removes a device from the list.
 *
 * @param head Head of the list to remove from
 * @param device Device to remove
 *
 * @retval 0 The device was successfully removed from the list
 * @retval -1 An error occurred and `errno` is set to indicate
 *            the cause
 */
int omemo_device_list_remove(struct device_list **head,
			     struct omemo_device *device);

/**
 * @brief Get the number of devices in the list.
 *
 * @param head Head of the list to count devices in
 *
 * @return Number of devices in list.
 */
uint32_t omemo_device_list_size(struct device_list **head);


/**
 * @brief Frees a device from the list.
 *
 * @param head Head of the list to free from
 * @param device Device to free
 *
 * @retval 0 The device was successfully removed from the list and freed
 * @retval -1 An error occurred and `errno` is set to indicate the cause
 */
int omemo_device_list_free_device(struct device_list **head,
				  struct omemo_device *device);

/**
 * @brief Frees the device list and all devices it contains.
 *
 * @param head Head of the list to free
 */
void omemo_device_list_free(struct device_list **head);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_DEVICE_LIST_H */

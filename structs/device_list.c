#include <errno.h>
#include <stdlib.h>
#include "device_list.h"
#include "omemo_device.h"


int omemo_device_list_add(struct device_list **head,
			  struct omemo_device *device)
{
	struct device_list *new_device;

	if (!head || !device) {
		errno = EINVAL;
		return -1;
	}

	/* Check if new device is already in list */
	if (omemo_device_list_contains(head, device)) {
		return 0;
	}


	new_device = malloc(sizeof(struct device_list));

	if (!new_device) {
		errno = ENOMEM;
		return -1;
	}

	new_device->next = *head;
	new_device->device = device;

	*head = new_device;

	return 1;
}

int omemo_device_list_add_inplace(struct device_list **head,
				  struct device_list *pos,
				  struct omemo_device *device)
{
	struct device_list *new_device, *cur;

	if (!head || !pos || !device) {
		errno = EINVAL;
		return -1;
	}
	
	if (!omemo_device_list_contains(head, pos->device)) {
		errno = EINVAL;
		return -1;
	}

	if (omemo_device_list_contains(head, device)) {
		return 0;
	}


	new_device = malloc(sizeof(struct device_list));

	if (!new_device) {
		errno = ENOMEM;
		return -1;
	}

	new_device->next = pos;
	new_device->device = device;

	if (*head == pos) {
		*head = new_device;
		return 1;
	}		

	for (cur = *head; cur->next != pos; cur = cur->next) { }

	cur->next = new_device;

	return 1;
}


int omemo_device_list_contains(struct device_list **head,
			       struct omemo_device *device)
{
	struct device_list *cur;

	if (!head || !device) {
		errno = EINVAL;
		return -1;
	}

	for (cur = *head; cur != NULL; cur = cur->next) {
		if (cur->device == device) {
			return 1;
		}
	}

	return 0;
}	


int omemo_device_list_contains_id(struct device_list **head,
				  uint32_t id)
{
	struct device_list *cur;

	if (!head || !id) {
		errno = EINVAL;
		return -1;
	}

	for (cur = *head; cur != NULL; cur = cur->next) {
		if (cur->device->id == id) {
			return 1;
		}
	}

	return 0;
}


int omemo_device_list_remove(struct device_list **head,
			     struct omemo_device *device)
{
	struct device_list *cur;

	if (!head || !device) {
		errno = EINVAL;
		return -1;
	}

	if ((*head)->device == device) {
		struct device_list *next = (*head)->next;
		free(*head);
		*head = next;
		return 0;
	}

	for (cur = *head; cur->next != NULL; cur = cur->next) {
		if (cur->next->device == device) {
			struct device_list *tmp = cur->next;
			cur->next = cur->next->next;
			free(tmp);
			return 0;
		}
	}

	errno = EINVAL;

	return -1;
}


uint32_t omemo_device_list_size(struct device_list **head)
{
	if (!head) {
		return 0;
	}

	uint32_t size = 0;
	struct device_list *cur;

	for (cur = *head; cur->next != NULL; cur = cur->next, size++)
	{ }

	return size;
}

int omemo_device_list_free_device(struct device_list **head,
				  struct omemo_device *device)
{
	int retval;
	retval = omemo_device_list_remove(head, device);

	if (retval < 0) {
		errno = EINVAL;
		return -1;
	}

	omemo_device_free(device);

	return 0;
}

void omemo_device_list_free(struct device_list **head)
{
	if (!head || !(*head)) {
		return;
	}

	while ((*head) != NULL) {
		struct device_list *tmp;
		omemo_device_free((*head)->device);

		tmp = (*head);
		*head = (*head)->next;
		free(tmp);		
	}
}

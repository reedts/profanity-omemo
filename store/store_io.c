#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <structs/device_list.h>
#include <structs/omemo_device.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <xmpp/omemo_constants.h>

#include "store_io.h"

static int omemo_mk_dir(const char *path)
{
	char path_buf[PATH_MAX];
	char *p = NULL;
	size_t len;

	len = snprintf(path_buf, sizeof(path_buf), "%s", path);
	if (path_buf[len - 1] == '/') {
		path_buf[len -1] = 0;
	}

	for(p = path_buf + 1; *p; ++p) {
		if (*p == '/') {
			*p = 0;
			if (mkdir(path_buf, S_IRWXU) != 0) {
				if (errno != EEXIST) {
					return -1;
				}
			}

			*p = '/';
		}
	}
	if (mkdir(path_buf, S_IRWXU) != 0) {
		if (errno != EEXIST) {
			return -1;
		}
	}

	return 0;
}


int omemo_store_device_list(const char *nick, struct device_list *list)
{
	char path[PATH_MAX];
	char *home;
	FILE *devices;
	size_t devices_written = 0;
	struct device_list *cur;
	struct stat st = {0};

	home = getenv("HOME");

	memset(path, 0x0, sizeof(path));

	strncpy(path, home, sizeof(path));
	strcat(path, OMEMO_WORKING_DIR);
	strncat(path, nick, sizeof(path) - strlen(path));
	
	/* if the device list belongs to someone else */
	if (strncmp(nick, list->device->jid, strlen(nick))) {
		strncat(path, "/contacts/", sizeof(path) - strlen(path));
		strncat(path, list->device->jid, strlen(list->device->jid));
	}

	puts(path);

	if (stat(path, &st) != 0) {
		if (errno == ENOENT) {
			if (omemo_mk_dir(path) != 0) {
				return -1;
			}
		} else {
			return -1;
		}
	}

	/* Expect no device list to exist */
	strcat(path, "/devices");

	devices = fopen(path, "w+");
	if (!devices) {
		return -1;
	}

	if (fputs("A:", devices) == EOF) {
		return -1;
	}

	for (cur = list; cur != NULL; cur = cur->next) {
		if (cur->device->status == ACTIVE) {
			if (devices_written) {
				fputs(",", devices);
			}
			fprintf(devices, "%d", cur->device->id);
			devices_written++;
		}
	}

	if (fputs("\nI:", devices) == EOF) {
		return -1;
	}
	
	devices_written = 0;
	for (cur = list; cur != NULL; cur = cur->next) {
		if (cur->device->status == INACTIVE) {
			if (devices_written) {
				fputs(",", devices);
			}
			fprintf(devices, "%d", cur->device->id);
			devices_written++;
		}
	}

	fclose(devices);

	return 0;

}

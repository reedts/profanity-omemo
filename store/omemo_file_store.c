#if _POSIX_C_SOURCE <= 200809L || !defined( _POSIX_C_SOURCE )
#define _POSIX_C_SOURCE 200809L
#endif

#if _XOPEN_SOURCE <= 500 || !defined( _XOPEN_SOURCE )
#define _XOPEN_SOURCE 500
#endif

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
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
#include <store/omemo_store.h>

static const char se_file_name[] = "session";
static const char dev_file_name[] = "devices";
static const char pre_key_folder_name[] = "pre_keys";
static const char signed_pre_key_folder_name[] = "signed_pre_keys";
static const char id_pub_key_file_name[] = "id_key.pub";
static const char id_priv_key_file_name[] = "id_key.priv";

/* extern */
struct omemo_store_context omemo_store_context = {
	.session_store = {
		.load_session_func		= omemo_load_session,
		.get_sub_device_sessions_func	= omemo_get_sub_device_sessions,
		.store_session_func		= omemo_store_session,
		.contains_session_func		= omemo_contains_session,
		.delete_session_func		= omemo_delete_session,
		.delete_all_sessions_func	= omemo_delete_all_sessions,
		.destroy_func			= omemo_session_store_destroy
	},
	.pre_key_store = {
		.load_pre_key			= omemo_load_pre_key,
		.store_pre_key			= omemo_store_pre_key,
		.contains_pre_key		= omemo_contains_pre_key,
		.remove_pre_key			= omemo_remove_pre_key,
		.destroy_func			= omemo_pre_key_store_destroy
	},
	.signed_pre_key_store = {
		.load_signed_pre_key		= omemo_load_signed_pre_key,
		.store_signed_pre_key		= omemo_store_signed_pre_key,
		.contains_signed_pre_key	= omemo_contains_signed_pre_key,
		.remove_signed_pre_key		= omemo_remove_signed_pre_key,
		.destroy_func			= omemo_signed_pre_key_store_destroy
	},
	.identity_key_store = {
		.get_identity_key_pair		= omemo_get_identity_key_pair,
		.get_local_registration_id	= omemo_get_local_registration_id,
		.save_identity			= omemo_save_identity,
		.is_trusted_identity		= omemo_is_trusted_identity,
		.destroy_func			= omemo_identity_key_store_destroy
	}
};


static int omemo_mk_dir(const char *path)
{
	char path_buf[PATH_MAX];
	char *p = NULL;
	size_t len;

	len = snprintf(path_buf, sizeof(path_buf), "%s", path);
	if (path_buf[len - 1] == '/') {
		path_buf[len - 1] = 0;
	} else {
		*strrchr(path_buf, '/') = 0;
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

/**
 * @brief Get the path to the pre key store to buffer
 *
 * @param local_user	Pointer to the address of the local user
 * @param buffer	Pointer to an allocated buffer
 * @param buf_len	Length of the buffer
 *
 * @retval 0		The path to pre key store was found and written to the
 *			buffer
 * @retval 1		The path to pre key store was not found but written to
 *			buffer
 * @retval -1		An error occurred and `errno` is set to indicate the
 *			cause
 */
static int omemo_get_pre_key_store(const signal_protocol_address *local_user,
				   char *buffer, size_t buf_len)
{
	size_t bytes_written;
	struct stat st = {0};
	
	if (!local_user || !buffer) {
		errno = EINVAL;
		return -1;
	}

	bytes_written = snprintf(buffer, buf_len, "%s/%s", getenv("HOME"),
				 OMEMO_WORKING_DIR);

	bytes_written += snprintf(buffer + bytes_written, local_user->name_len,
				  "/%s", local_user->name);

	bytes_written += snprintf(buffer + bytes_written,
				  buf_len - bytes_written,
				  "/%d/%s", local_user->device_id,
				  pre_key_folder_name);	
	
	if (bytes_written > buf_len) {
		errno = ENOBUFS;
		return -1;
	}

	return (stat(buffer, &st) < 0) ? 1 : 0;
}

/**
 * @brief Get the path to the signed pre key store to buffer
 *
 * @param local_user	Pointer to the address of the local user
 * @param buffer	Pointer to an allocated buffer
 * @param buf_len	Length of the buffer
 *
 * @retval 0		The path to signed pre key store was found and written
 *			to the buffer
 * @retval 1		The path to signed pre key store was not found but
 *			written to buffer
 * @retval -1		An error occurred and `errno` is set to indicate the
 *			cause
 */
static int
omemo_get_signed_pre_key_store(const signal_protocol_address *local_user,
			       char *buffer, size_t buf_len)
{
	size_t bytes_written;
	struct stat st = {0};
	
	if (!local_user || !buffer) {
		errno = EINVAL;
		return -1;
	}

	bytes_written = snprintf(buffer, buf_len, "%s/%s", getenv("HOME"),
				 OMEMO_WORKING_DIR);

	bytes_written += snprintf(buffer + bytes_written, local_user->name_len,
				  "/%s", local_user->name);

	bytes_written += snprintf(buffer + bytes_written,
				  buf_len - bytes_written,
				  "/%d/%s", local_user->device_id,
				  signed_pre_key_folder_name);	
	
	if (bytes_written > buf_len) {
		errno = ENOBUFS;
		return -1;
	}

	return (stat(buffer, &st) < 0) ? 1 : 0;
}

static int omemo_get_identity_key_store(const signal_protocol_address *local_user,
					char *buffer, size_t buf_len)
{
	size_t bytes_written;
	struct stat st = {0};

	if (!local_user || !buffer) {
		errno = EINVAL;
		return -1;
	}

	bytes_written = snprintf(buffer, buf_len, "%s/%s", getenv("HOME"),
				 OMEMO_WORKING_DIR);

	bytes_written += snprintf(buffer + bytes_written, local_user->name_len,
				  "/%s", local_user->name);

	bytes_written += snprintf(buffer + bytes_written, buf_len - bytes_written,
				  "%d/", local_user->device_id);

	if (bytes_written > buf_len) {
		errno = ENOBUFS;
		return -1;
	}

	return (stat(buffer, &st) < 0) ? 1: 0;
}

/**
 * @brief Checks if a directory in the OMEMO directory structure exists
 *	  and returns the path.
 *
 * @param local_user	The local OMEMO user
 * @param user		The user whose directory to find or `NULL` if the directory
 *			of the local user should be found
 * @param file		The file's name to find or `NULL` to only look for
 *			the directory
 * @param buffer	The allocated buffer to write the path to
 * @param buf_len	The lenght of the buffer in bytes
 *
 * @retval 0		The directory was found and correctly written to buffer
 * @retval 1		The directory was not found but the correct path was
 *			written to the buffer
 * @retval -1		An error occurred and errno is set to indicate the
 *			cause
 */

static int omemo_get_dir(const signal_protocol_address *local_user,
			 const signal_protocol_address *user,
			 const char *file, char *buffer, size_t buf_len)
{
	if (!local_user || !buffer || !buf_len) {
		errno = EINVAL;
		return -1;
	}

	size_t bytes_written;
	struct stat st = {0};

	bytes_written = snprintf(buffer, buf_len, "%s/%s/%s/%d",
				 getenv("HOME"), OMEMO_WORKING_DIR, 
				 local_user->name, local_user->device_id);

	if (!user || !strncmp(local_user->name, user->name,
			      local_user->name_len)) {
		/* local user and searched user are the same */
		if (file) {
			bytes_written += snprintf(buffer + bytes_written,
						  buf_len - bytes_written,
						  "/%s", file);
		}
	} else {
		bytes_written += (!strcmp(file, dev_file_name)
				  || !user->device_id)
			? snprintf(buffer+bytes_written, buf_len-bytes_written,
				   "/%s/%s", "contacts", user->name)
			: snprintf(buffer+bytes_written, buf_len-bytes_written,
				   "/%s/%s/%d", "contacts", user->name,
				   user->device_id)
			;

		if (file) {
			bytes_written += snprintf(buffer + bytes_written,
						  buf_len - bytes_written,
						  "/%s", file);
		}
	}

	if (bytes_written >= buf_len) {
		errno = ENOBUFS;
		return -1;
	}
	if (stat(buffer, &st) < 0) {
		return 1;
	}
	
	return 0;
}

/**
 * @brief Deletes a node in file tree walk
 *
 * @retval 0	The node was removed successfully
 * @retval -1	An error occurred while removing the node
 */
int omemo_delete_cb(const char *fpath, const struct stat *sb, int typeflag,
		    struct FTW *ftwbuf)
{
	if (remove(fpath))
		return -1;
	else
		return 0;
}

/**
 * @brief Deletes a directory recursively
 *
 * @param path	The path to delete
 * 
 * @retval 0	The directory was removed successfully
 * @retval -1	An error occurred while removing the directory
 */
int omemo_rm_dir(const char *path)
{
	return nftw(path, omemo_delete_cb, 64, FTW_DEPTH | FTW_PHYS);
}


int omemo_store_device_list(const signal_protocol_address *user,
			    struct device_list *list)
{
	char path[PATH_MAX];
	FILE *devices;
	int retval;
	size_t devices_written = 0;
	struct device_list *cur;
	
	retval = omemo_get_dir(user, &(list->device->address), dev_file_name,
			       path, sizeof(path));
	
	puts(path);
	if (retval < 0) {
		return -1;
	}

	if (retval) {
		if (omemo_mk_dir(path) != 0) {
			return -1;
		}
	}

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
			fprintf(devices, "%d", cur->device->address.device_id);
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
			fprintf(devices, "%d", cur->device->address.device_id);
			devices_written++;
		}
	}

	fclose(devices);

	return 0;

}

int omemo_is_local_user_existent(const signal_protocol_address *address)
{
	int retval;
	char buffer[PATH_MAX];

	if (!address) {
		errno = EINVAL;
		return -1;
	}

	retval = omemo_get_dir(address, NULL, NULL, buffer, sizeof(buffer));
	if (retval < 0) {
		return -1;
	}

	return !retval;
}


int omemo_load_session(signal_buffer **record,
		       const signal_protocol_address *address, void *user_data)
{
	if (!address) {
		return SG_ERR_INVAL;
	}

	int retval;
	char path_buf[PATH_MAX];
	FILE *se_file = NULL;
	uint8_t *se_data;
	struct stat st = {0};

	/* TODO: Get the JID of the current user here. We probably have to save
	 * it with the OMEMO state as we might only get it when the plugin is
	 * initialised.
	 */
	retval = omemo_get_dir(NULL, address, se_file_name, path_buf,
			       sizeof(path_buf));

	if (retval <= 0) {
		return (retval == 0) ? retval : SG_ERR_UNKNOWN;
	}

	se_file = fopen(path_buf, "r");
	if (!se_file) {
		return SG_ERR_UNKNOWN;
	}
	
	/* Determine size of file */
	if (fstat(fileno(se_file), &st)) {
		fclose(se_file);
		return SG_ERR_UNKNOWN;
	}

	se_data = malloc(st.st_size);
	if (!se_data) {
		fclose(se_file);
		return SG_ERR_NOMEM;
	}

	if (fread(se_data, 1, st.st_size, se_file) != st.st_size) {
		goto err_cleanup;
	}

	*record = signal_buffer_create(se_data, st.st_size);
	if (!(*record)) {
		goto err_cleanup;
	}

	fclose(se_file);
	free(se_data);
	return 1;

err_cleanup:
	fclose(se_file);
	free(se_data);
	return SG_ERR_UNKNOWN;
}


int omemo_get_sub_device_sessions(signal_int_list **sessions, const char *name,
				  size_t name_len, void *user_data)
{
	char buffer[PATH_MAX];
	DIR *dir;
	int size, retval;
	struct dirent *folder;
	struct signal_protocol_address user_addr = {
		.name = name,
		.name_len = name_len,
		.device_id = 0
	};

	size = 0;

	if (!name)
		return SG_ERR_INVAL;
	
	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_dir(NULL, &user_addr, NULL, buffer, sizeof(buffer));
	if (retval < 0) {
		return SG_ERR_UNKNOWN;
	} else if (!retval) {
		return 0;
	}

	dir = opendir(buffer);
	if (!dir) 
		return SG_ERR_UNKNOWN;
	
	*sessions = signal_int_list_alloc();
	if (!*sessions) 
		return SG_ERR_UNKNOWN;

	while ((folder = readdir(dir))) {
		int device_id = atoi(folder->d_name);
		signal_int_list_push_back(*sessions, device_id);
		++size;
	}
	
	closedir(dir);
	return size;
}

int omemo_store_session(const signal_protocol_address *address, uint8_t *record,
			size_t record_len, void *user_data)
{
	char buffer[PATH_MAX];
	FILE *se_file;
	int retval;
	size_t bytes_written;

	if (!address || !record)
		return SG_ERR_INVAL;
	
	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: get local user! */
	retval = omemo_get_dir(NULL, address, se_file_name, buffer,
			       sizeof(buffer));
	if (retval < 0) 
		return SG_ERR_UNKNOWN;
	
	se_file = fopen(buffer, "w");
	if (!se_file)
		return SG_ERR_UNKNOWN;

	bytes_written = fwrite(record, 1, record_len, se_file);
	if (bytes_written < record_len) {
		fclose(se_file);
		return SG_ERR_UNKNOWN;
	}

	fclose(se_file);
	return 0;
}

int omemo_contains_session(const signal_protocol_address *address,
			   void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	
	if (!address)
		return SG_ERR_INVAL;

	/* TODO: get local user! */
	retval = omemo_get_dir(NULL, address, NULL, buffer, sizeof(buffer));

	return (retval == 0) ? 1 : 0;
}

int omemo_delete_session(const signal_protocol_address *address,
			 void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	
	if (!address)
		return SG_ERR_INVAL;

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: get local user! */
	retval = omemo_get_dir(NULL, address, NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval)
		return 0;

	return omemo_rm_dir(buffer);
}

int omemo_delete_all_sessions(const char *name, size_t name_len,
			      void *user_data)
{
	char buffer[PATH_MAX];
	DIR *dir;
	int retval, se_deleted;
	struct dirent *folder;
	signal_protocol_address user_addr = {
		.name = name,
		.name_len = name_len,
		.device_id = 0
	};

	se_deleted = 0;

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: get local user! */
	retval = omemo_get_dir(NULL, &user_addr, NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval)
		return 0;

	dir = opendir(buffer);
	if (!dir)
		return SG_ERR_UNKNOWN;

	while ((folder = readdir(dir)))
		++se_deleted;
	
	retval = omemo_rm_dir(buffer);
	if (retval < 0)
		return retval;
	else
		return se_deleted;
}

void omemo_session_store_destroy(void *user_data)
{ }

signal_protocol_session_store omemo_session_store = {
	.load_session_func		= omemo_load_session,
	.get_sub_device_sessions_func	= omemo_get_sub_device_sessions,
	.store_session_func		= omemo_store_session,
	.contains_session_func		= omemo_contains_session,
	.delete_session_func		= omemo_delete_session,
	.delete_all_sessions_func	= omemo_delete_all_sessions,
	.destroy_func			= omemo_session_store_destroy
};


int omemo_load_pre_key(signal_buffer **record, uint32_t pre_key_id,
		       void *user_data)
{
	char path_buf[PATH_MAX];
	FILE *pk_file;
	int retval;
	struct stat st;
	uint8_t *buffer;

	memset(path_buf, 0x0, sizeof(path_buf));
	memset(&st, 0x0, sizeof(struct stat));
	
	/* TODO: Get local user! */
	retval = omemo_get_pre_key_store(NULL, path_buf, sizeof(path_buf));
	if (retval)
		return SG_ERR_INVALID_KEY_ID;
	
	pk_file = fopen(path_buf, "r");
	if (!pk_file)
		return SG_ERR_INVALID_KEY_ID;

	/* Determine file size */
	if (fstat(fileno(pk_file), &st) < 0) {
		fclose(pk_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	buffer = malloc(st.st_size);
	if (!buffer) {
		fclose(pk_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	if (fread(buffer, 1, st.st_size, pk_file) < st.st_size)
		goto err_cleanup;

	*record = signal_buffer_create(buffer, st.st_size);
	if (!(*record))
		goto err_cleanup;

	fclose(pk_file);
	free(buffer);
	return SG_SUCCESS;
	
err_cleanup:
	
	fclose(pk_file);
	free(buffer);
	return SG_ERR_INVALID_KEY_ID;
}

int omemo_store_pre_key(uint32_t pre_key_id, uint8_t *record, size_t record_len,
			void *user_data)
{
	char buffer[PATH_MAX];
	FILE *pk_file;
	int retval;
	size_t bytes_free, bytes_written;

	if (!record)
		return SG_ERR_INVAL;

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval) {
		retval = omemo_mk_dir(buffer);
		if (retval < 0)
			return SG_ERR_UNKNOWN;
	}
	
	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", pre_key_id) >= bytes_free)
		return SG_ERR_UNKNOWN;

	pk_file = fopen(buffer, "w");
	if (!pk_file)
		return SG_ERR_UNKNOWN;

	bytes_written = fwrite(record, 1, record_len, pk_file);
	if (bytes_written < record_len) {
		fclose(pk_file);
		return SG_ERR_UNKNOWN;
	}

	fclose(pk_file);
	return 0;
}

int omemo_contains_pre_key(uint32_t pre_key_id, void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	size_t bytes_free;
	struct stat st = {0};

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval)
		return 0;

	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", pre_key_id) >= bytes_free)
		return SG_ERR_UNKNOWN;

	return (stat(buffer, &st) < 0) ? 0 : 1;
}

int omemo_remove_pre_key(uint32_t pre_key_id, void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	size_t bytes_free;
	struct stat st = {0};

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval)
		return SG_ERR_UNKNOWN;

	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", pre_key_id) >= bytes_free)
		return SG_ERR_UNKNOWN;

	if (stat(buffer, &st) < 0)
		return SG_ERR_UNKNOWN;

	retval = remove(buffer);
	if (retval < 0)
		return SG_ERR_UNKNOWN;

	return 0;
}

void omemo_pre_key_store_destroy(void *user_data)
{ }


int omemo_load_signed_pre_key(signal_buffer **record,
			      uint32_t signed_pre_key_id, void *user_data)
{
	char path_buf[PATH_MAX];
	FILE *pk_file;
	int retval;
	struct stat st;
	uint8_t *buffer;

	memset(path_buf, 0x0, sizeof(path_buf));
	memset(&st, 0x0, sizeof(struct stat));
	
	/* TODO: Get local user! */
	retval = omemo_get_signed_pre_key_store(NULL, path_buf,
						sizeof(path_buf));
	if (retval)
		return SG_ERR_INVALID_KEY_ID;
	
	pk_file = fopen(path_buf, "r");
	if (!pk_file)
		return SG_ERR_INVALID_KEY_ID;

	/* Determine file size */
	if (fstat(fileno(pk_file), &st) < 0) {
		fclose(pk_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	buffer = malloc(st.st_size);
	if (!buffer) {
		fclose(pk_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	if (fread(buffer, 1, st.st_size, pk_file) < st.st_size)
		goto err_cleanup;

	*record = signal_buffer_create(buffer, st.st_size);
	if (!(*record))
		goto err_cleanup;

	fclose(pk_file);
	free(buffer);
	return SG_SUCCESS;
	
err_cleanup:
	
	fclose(pk_file);
	free(buffer);
	return SG_ERR_INVALID_KEY_ID;
}

int omemo_store_signed_pre_key(uint32_t signed_pre_key_id, uint8_t *record,
			       size_t record_len, void *user_data)
{
	char buffer[PATH_MAX];
	FILE *pk_file;
	int retval;
	size_t bytes_free, bytes_written;

	if (!record)
		return SG_ERR_INVAL;

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_signed_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval) {
		retval = omemo_mk_dir(buffer);
		if (retval < 0)
			return SG_ERR_UNKNOWN;
	}
	
	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", signed_pre_key_id)
	    >= bytes_free)
		return SG_ERR_UNKNOWN;

	pk_file = fopen(buffer, "w");
	if (!pk_file)
		return SG_ERR_UNKNOWN;

	bytes_written = fwrite(record, 1, record_len, pk_file);
	if (bytes_written < record_len) {
		fclose(pk_file);
		return SG_ERR_UNKNOWN;
	}

	fclose(pk_file);
	return 0;
}

int omemo_contains_signed_pre_key(uint32_t signed_pre_key_id, void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	size_t bytes_free;
	struct stat st = {0};

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_signed_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval < 0)
		return SG_ERR_UNKNOWN;
	else if (retval)
		return 0;

	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", signed_pre_key_id)
	    >= bytes_free)
		return SG_ERR_UNKNOWN;

	return (stat(buffer, &st) < 0) ? 0 : 1;
}

int omemo_remove_signed_pre_key(uint32_t signed_pre_key_id, void *user_data)
{
	char buffer[PATH_MAX];
	int retval;
	size_t bytes_free;
	struct stat st = {0};

	memset(buffer, 0x0, sizeof(buffer));
	
	/* TODO: Get local user! */
	retval = omemo_get_signed_pre_key_store(NULL, buffer, sizeof(buffer));
	if (retval)
		return SG_ERR_UNKNOWN;

	bytes_free = sizeof(buffer) - strlen(buffer);
	if (snprintf(buffer, bytes_free, "/%d", signed_pre_key_id)
	    >= bytes_free)
		return SG_ERR_UNKNOWN;

	if (stat(buffer, &st) < 0)
		return SG_ERR_UNKNOWN;

	retval = remove(buffer);
	if (retval < 0)
		return SG_ERR_UNKNOWN;

	return 0;
}

void omemo_signed_pre_key_store_destroy(void *user_data)
{ }


int omemo_get_identity_key_pair(signal_buffer **public_data, signal_buffer **private_data,
				void *user_data)
{
	char buffer[PATH_MAX];
	FILE *priv_key_file;
	FILE *pub_key_file;
	int retval;
	size_t path_len;
	size_t bytes_free;
	uint8_t *priv_key_buffer;
	uint8_t *pub_key_buffer;
	struct stat st = {0};

	memset(buffer, 0x0, sizeof(buffer));

	/* TODO: Get local user! */
	retval = omemo_get_identity_key_store(NULL, buffer, sizeof(buffer));
	if (retval)
		return SG_ERR_UNKNOWN;

	path_len = strlen(buffer);
	bytes_free = sizeof(buffer) - path_len;
	
	/* Load private key */
	if (snprintf(buffer + path_len, bytes_free, "/%s", id_priv_key_file_name)
	    >= bytes_free) {
		return SG_ERR_UNKNOWN;
	}

	priv_key_file = fopen(buffer, "r");
	if (!priv_key_file) {
		return SG_ERR_INVALID_KEY_ID;
	}

	if (fstat(fileno(priv_key_file), &st) < 0) {
		fclose(priv_key_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	priv_key_buffer = malloc(st.st_size);
	if (!priv_key_buffer) {
		fclose(priv_key_file);
		return SG_ERR_UNKNOWN;
	}

	if (fread(priv_key_buffer, 1, st.st_size, priv_key_file) < st.st_size) {
		fclose(priv_key_file);
		free(priv_key_buffer);
		return SG_ERR_UNKNOWN;
	}

	*private_data = signal_buffer_create(priv_key_buffer, st.st_size);
	if (!(*private_data)) {
		fclose(priv_key_file);
		free(priv_key_buffer);
		return SG_ERR_UNKNOWN;
	}
	
	fclose(priv_key_file);

	/* Load public key */
	if (snprintf(buffer + path_len, bytes_free, "/%s", id_pub_key_file_name)
	    >= bytes_free) {
		return SG_ERR_UNKNOWN;
	}
	
	pub_key_file = fopen(buffer, "r");
	if (!pub_key_file) {
		return SG_ERR_INVALID_KEY_ID;
	}

	if (fstat(fileno(pub_key_file), &st) < 0) {
		fclose(pub_key_file);
		return SG_ERR_INVALID_KEY_ID;
	}

	pub_key_buffer = malloc(st.st_size);
	if (!pub_key_buffer) {
		fclose(pub_key_file);
		return SG_ERR_UNKNOWN;
	}

	if (fread(pub_key_buffer, 1, st.st_size, pub_key_file) < st.st_size) {
		fclose(pub_key_file);
		free(pub_key_buffer);
		return SG_ERR_UNKNOWN;
	}

	*public_data = signal_buffer_create(pub_key_buffer, st.st_size);
	if (!(*public_data)) {
		fclose(pub_key_file);
		free(pub_key_buffer);
		return SG_ERR_UNKNOWN;
	}
	
	fclose(pub_key_file);
	
	return SG_SUCCESS;
}

int omemo_get_local_registration_id(void *user_data, uint32_t *registration_id)
{
	if (!registration_id) {
		return SG_ERR_INVAL;
	}

	/* default value for OMEMO, only important for signal */
	*registration_id = 0;

	return SG_SUCCESS;
}

int omemo_save_identity(const signal_protocol_address *address, uint8_t *key_data,
			size_t key_len, void *user_data)
{
	/* Not needed as OMEMO handles device management */

	return SG_SUCCESS;
}

int omemo_is_trusted_identity(const signal_protocol_address *address, uint8_t *key_data,
			      size_t key_len, void *user_data)
{
	/* Trust management is done by OMEMO */
	return 1;
}

void omemo_identity_key_store_destroy(void *user_data)
{
}

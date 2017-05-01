#ifndef PROF_OMEMO_STORE_IO_H
#define PROF_OMEMO_STORE_IO_H


#include <signal_protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device_list;

extern struct signal_protocol_session_store omemo_session_store;

/**
 * @brief Stores a device list persistently
 *
 * @param user Local user
 * @param list Device list to store
 * 
 * @retval 0 The device list was stored successfully
 * @retval -1 An error occurred and `errno` was set to indicate the cause
 */
int omemo_store_device_list(const signal_protocol_address *user, struct device_list *list);


/* From Libsignal for signal_protocol_session_store */
int omemo_load_session(signal_buffer **record,
		       const signal_protocol_address *address, void *user_data);

int omemo_get_sub_device_sessions(signal_int_list **sessions,
				  const char *name, size_t name_len,
				  void *user_data);

int omemo_store_session(const signal_protocol_address *address, uint8_t *record,
			size_t record_len, void *user_data);

int omemo_contains_session(const signal_protocol_address *address,
			   void *user_data);

int omemo_delete_session(const signal_protocol_address *address,
			 void *user_data);

int omemo_delete_all_sessions(const char *name, size_t name_len,
			      void *user_data);

void omemo_session_store_destroy(void *user_data);


#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_STORE_IO_H */

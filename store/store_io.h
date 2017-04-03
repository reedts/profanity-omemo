#ifndef PROF_OMEMO_STORE_IO_H
#define PROF_OMEMO_STORE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

struct device_list;

/**
 * @brief Stores a device list persistently
 *
 * @param list Device list to store
 * 
 * @retval 0 The device list was stored successfully
 * @retval -1 An error occurred and `errno` was set to indicate the cause
 */
int omemo_store_device_list(struct device_list *list);

#ifdef __cplusplus
}
#endif

#endif /* PROF_OMEMO_STORE_IO_H */

#ifndef SVR_CORE_UTILS_H_
#define SVR_CORE_UTILS_H_

#define BUFFER_SIZE 1024 * 4
#define TIMEOUT_SEC 5

#define SHM_DEFAULT_OPEN_FLAG 0666


char*
create_shm(key_t key, int size, int flags);

#endif // SVR_CORE_UTILS_H_

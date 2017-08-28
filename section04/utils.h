#ifndef SVR_CORE_UTILS_H_
#define SVR_CORE_UTILS_H_

#define BUFFER_SIZE 4 * 1024
#define TIMEOUT_SEC 1

#define SHM_DEFAULT_OPEN_FLAG 0666
#define CONNECTION_NUM 100*1024

char*
create_shm(key_t key, int size, int flags);

#endif // SVR_CORE_UTILS_H_

#include "lmdb.h"
#include "midl.h"

int mdb_env_open_readonly(
    MDB_env *env,
    int fd,
    // const char *path,
    unsigned int flags,
    mdb_mode_t mode);
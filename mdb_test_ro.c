#include "mdb_custom.h"
#include <stdio.h>


#include <fcntl.h>
int main(){
    MDB_env *env;
    const char* file = "testdb/data.mdb";
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        printf("open failed\n");
        return 1;
    }
    unsigned int flags = MDB_RDONLY;
    mdb_mode_t mode = 0;
    int rc = mdb_env_create(&env);
    printf("rc: %d\n", rc);
    rc = mdb_env_open_readonly(env, fd, flags, mode);
    printf("rc: %d\n", rc);
    printf("rc: %s\n", mdb_strerror(rc));

    // iterate over all databases
    MDB_txn *txn;
    MDB_dbi dbi;
    MDB_cursor *cursor;
    MDB_val key;
    MDB_val data;
    rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    printf("rc: %d\n", rc);
    rc = mdb_cursor_open(txn, dbi, &cursor);
    printf("rc: %d\n", rc);
    while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
        printf("key: %s, ", (const char *)key.mv_data);
        printf("data len: %zu\n", data.mv_size);
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
    mdb_env_close(env);
    printf("rc: %d\n", rc);
    printf("rc: %s\n", mdb_strerror(rc));
    return 0;
}
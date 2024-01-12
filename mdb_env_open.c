#include "lmdb/libraries/liblmdb/mdb.c"

int ESECT
mdb_env_open_readonly(
	MDB_env *env, 
	HANDLE fd,
	// const char *path, 
	unsigned int flags, 
	mdb_mode_t mode)
{
	/* make readonly */
	flags |= MDB_RDONLY | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NOLOCK;
	flags &= ~MDB_WRITEMAP;

	int rc = 0, excl = -1;

	if (env->me_fd!=INVALID_HANDLE_VALUE || (flags & ~(CHANGEABLE|CHANGELESS)))
		return EINVAL;

#ifdef MDB_VL32
	if (flags & MDB_WRITEMAP) {
		/* silently ignore WRITEMAP in 32 bit mode */
		flags ^= MDB_WRITEMAP;
	}
	if (flags & MDB_FIXEDMAP) {
		/* cannot support FIXEDMAP */
		return EINVAL;
	}
#endif
	flags |= env->me_flags;

#ifdef MDB_VL32
#ifdef _WIN32
	env->me_rpmutex = CreateMutex(NULL, FALSE, NULL);
	if (!env->me_rpmutex) {
		rc = ErrCode();
		goto leave;
	}
#else
	rc = pthread_mutex_init(&env->me_rpmutex, NULL);
	if (rc)
		goto leave;
#endif
#endif

	flags |= MDB_ENV_ACTIVE;	/* tell mdb_env_close0() to clean up */

	env->me_flags = flags;
	if (rc)
		goto leave;

#ifdef MDB_VL32
	{
		env->me_rpages = malloc(MDB_ERPAGE_SIZE * sizeof(MDB_ID3));
		if (!env->me_rpages) {
			rc = ENOMEM;
			goto leave;
		}
		env->me_rpages[0].mid = 0;
		env->me_rpcheck = MDB_ERPAGE_SIZE/2;
	}
#endif

	env->me_path = strdup("/tmp/mapped_memory_aaabbb");
	env->me_dbxs = calloc(env->me_maxdbs, sizeof(MDB_dbx));
	env->me_dbflags = calloc(env->me_maxdbs, sizeof(uint16_t));
	env->me_dbiseqs = calloc(env->me_maxdbs, sizeof(unsigned int));
	if (!(env->me_dbxs && env->me_path && env->me_dbflags && env->me_dbiseqs)) {
		rc = ENOMEM;
		goto leave;
	}
	env->me_dbxs[FREE_DBI].md_cmp = mdb_cmp_long; /* aligned MDB_INTEGERKEY */

	if (fd == INVALID_HANDLE_VALUE){
		rc = EINVAL;
		goto leave;
	}
	env->me_fd = fd;
	
#ifdef _WIN32
	rc = mdb_fopen(env, &fname, MDB_O_OVERLAPPED, mode, &env->me_ovfd);
	if (rc)
		goto leave;
#endif

	if ((rc = mdb_env_open2(env, flags & MDB_PREVSNAPSHOT)) == MDB_SUCCESS) {
		/* Synchronous fd for meta writes. Needed even with
		 * MDB_NOSYNC/MDB_NOMETASYNC, in case these get reset.
		 */
		DPRINTF(("opened dbenv %p", (void *) env));
	}

leave:
	MDB_TRACE(("%p, %s, %u, %04o", env, path, flags & (CHANGEABLE|CHANGELESS), mode));
	if (rc) {
		mdb_env_close0(env, excl);
	}
	return rc;
}
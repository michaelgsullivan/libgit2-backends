/*
	if( error = mdb_env_stat(backend->env, &backend->env) != MDB_OK )
		return error;
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * In addition to the permissions in the GNU General Public License,
 * the authors give you unlimited permission to link the compiled
 * version of this file into combinations with other programs,
 * and to distribute those combinations without any restriction
 * coming from the use of this file.  (The General Public License
 * restrictions do apply in other respects; for example, they cover
 * modification of the file, and distribution when not linked into
 * a combined executable.)
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <assert.h>
#include <string.h>
#include <git2.h>
#include <git2/sys/odb_backend.h>
#include <lmdb.h>

#define GIT2_TABLE_NAME "git2_odb"
#define MDB_OK 0

typedef struct {
	git_odb_backend parent;
	MDB_env *env;
	MDB_dbi obj_dbi;
	MDB_dbi ref_dbi;
	MDB_dbi meta_dbi;
	MDB_stat env_stat;
	MDB_envinfo env_info;
	MDB_txn *cur_txn;
} mdb_backend;

int mdb_backend__read_header(size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
	mdb_backend *backend;
	int error;

	assert(len_p && type_p && _backend && oid);

	backend = (mdb_backend *)_backend;
	error = GIT_ERROR;

	MDB_txn *txn = backend->cur_txn;
	unsigned int tx_flags = MDB_RDONLY | MDB_NOSYNC;
	if( !txn && error = mdb_txn_begin(backend->env, NULL, tx_flags, &txn) != MDB_OK )
		return error;

        struct MDB_val oid_val, obj_val;
        oid_val.mv_size = 20; // oid's are 20 bytes in libgit2
        oid_val.mv_data = oid;

        if( error = mdb_get(txn, backend->obj_dbi, &oid, &data) != MDB_OK ) {
                mdb_txn_commit(txn);
		return error;
        }

	*len_p = obj_val.mv_size;
	 


	if(!backend->cur_txn)
		mdb_txn_commit(txn);

	return error;
}

int mdb_backend__read(void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend, const git_oid *oid)
{
	mdb_backend *backend;
	int error;

	assert(data_p && len_p && type_p && _backend && oid);

	backend = (mdb_backend *)_backend;
	error = GIT_ERROR;

	MDB_txn *txn = backend->cur_txn;
	unsigned int tx_flags = MDB_RDONLY | MDB_NOSYNC;
	if( !txn && error = mdb_txn_begin(backend->env, NULL, tx_flags, &txn) != MDB_OK )
		return error;

	struct MDB_val oid_val, obj_val;
	oid_val.mv_size = 20; // oid's are 20 bytes in libgit2
	oid_val.mv_data = oid;  

	int found;
	if( error = mdb_get(txn, backend->obj_dbi, &oid, &data) != MDB_OK ) {
		found = 0;
	} else {
		found = 1;
	}

	if(!backend->cur_txn)
		mdb_txn_commit(txn);

	return error;
}

int mdb_backend__read_prefix(git_oid *out_oid, void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *_backend,
					const git_oid *short_oid, size_t len) {
	MDB_txn *txn = backend->cur_txn;
	unsigned int tx_flags = MDB_RDONLY | MDB_NOSYNC;
	if( !txn && error = mdb_txn_begin(backend->env, NULL, tx_flags, &txn) != MDB_OK )
		return error;


	if(!backend->cur_txn)
		mdb_txn_commit(txn);
	/* not implemented (yet) */
	return GIT_ERROR;
}

int mdb_backend__exists(git_odb_backend *_backend, const git_oid *oid)
{
	mdb_backend *backend;
	int found;

	assert(_backend && oid);

	backend = (mdb_backend *)_backend;
	found = 0;
	
	MDB_txn *txn = backend->cur_txn;
	unsigned int tx_flags = MDB_RDONLY | MDB_NOSYNC;
	if( !txn && error = mdb_txn_begin(backend->env, NULL, tx_flags, &txn) != MDB_OK )
		return error;

	struct MDB_val oid_val, obj_val;
	oid_val.mv_size = 20; // oid's are 20 bytes in libgit2
	oid_val.mv_data = oid;  

	if( error = mdb_get(txn, backend->obj_dbi, &oid, &data) != MDB_OK ) {
		found = 0;
	} else {
		found = 1;
	}

	if(!backend->cur_txn)
		mdb_txn_commit(txn);

	return found;
}


int mdb_backend__write(git_odb_backend *_backend, const git_oid *id, const void *data, size_t len, git_otype type)
{
	int error = MDB_OK;
	mdb_backend *backend;

	assert(id && _backend && data);

	backend = (mdb_backend *)_backend;

	MDB_txn *txn = backend->cur_txn;
	unsigned int tx_flags = NULL;
	if( !txn && error = mdb_txn_begin(backend->env, NULL, tx_flags, &txn) != MDB_OK )
		return error;


	if(!backend->cur_txn)
		mdb_txn_commit(txn);
	
	return error;
}


void mdb_backend__free(git_odb_backend *_backend)
{
	mdb_backend *backend;
	assert(_backend);
	backend = (mdb_backend *)_backend;

	if( backend->cur_tx ) mdb_txn_commit(backend->cur_tx);
	backend->cur_tx = NULL;

	mdb_env_close(backend->env);

	free(backend);
}

#define MDB_MAXDATABASES 125
#define MDB_CALL(stmt) if( error = stmt != MDB_OK ) \
	return error;

static int mdb_backend__env_open(const char *path, MDB_env **env){
	mdb_mode_t mode = 0640; // -rw-r----- file permissions
	unsigned int flags = MDB_NOSUBDIR;

	int error = MDB_OK;

	// Create the handle
	if( error = mdb_env_create(env) != MDB_OK)
		return error;

	// Set max databases per env
	if( error = mdb_env_set_maxdbs(*env, MDB_MAXDATABASES) != MDB_OK )
		return error;
	
	// Open the handle
	if( error = mdb_env_open(*env, path, flags, mode) != MDB_OK){
		mdb_env_close(*env);
		return error;
	}

	return MDB_OK;
}



static int init_mdb_backend(mdb_backend *backend, const char *path)
{
	int error = MDB_OK;

	MDB_CALL( mdb_backend__env_open(path, backend->env) )

	if( error = mdb_env_stat(backend->env, &backend->env_stat) != MDB_OK )
		return error;

	if( error = mdb_env_info(backend->env, &backend->env_info) != MDB_OK )
		return error;

	backend->cur_txn = NULL;

	unsigned int flags = MDB_CREATE;

 	MDB_CALL( mdb_dbi_open(tx, "@objs", flags, &backend->obj_dbi) )
	
 	MDB_CALL( mdb_dbi_open(tx, "@refs", flags | MDB_DUPSORT, &backend->ref_dbi) )
 	
	MDB_CALL( mdb_dbi_open(tx, "@meta", flags | MDB_DUPSORT, &backend->meta_dbi) )

	
	
	return error;
}


int git_odb_backend_mdb(git_odb_backend **backend_out, const char *mdb_file)
{
	mdb_backend *backend;
	int error;

	backend = calloc(1, sizeof(mdb_backend));
	if (backend == NULL) {
		giterr_set_oom();
		return GIT_ERROR;
	}
	
	if (init_mdb_backend(backend, mdb_file) != MDB_OK)
		goto cleanup;

	backend->parent.version = GIT_ODB_BACKEND_VERSION;
	backend->parent.read = &mdb_backend__read;
	backend->parent.read_prefix = &mdb_backend__read_prefix;
	backend->parent.read_header = &mdb_backend__read_header;
	backend->parent.write = &mdb_backend__write;
	backend->parent.exists = &mdb_backend__exists;
	backend->parent.free = &mdb_backend__free;

	*backend_out = (git_odb_backend *)backend;

	return GIT_OK;

cleanup:
	mdb_backend__free((git_odb_backend *)backend);
	return error;
}

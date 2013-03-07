/** Copyleft (ɔ) meh. - http://meh.schizofreni.co
 *
 * This file is part of hulk - https://github.com/meh/hulk
 *
 * hulk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * hulk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with hulk. If not, see <http://www.gnu.org/licenses/>.
 */

#include <hulk.h>
#include "ext.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef
struct super_block_t {
	int32_t inodes_count;
	int32_t blocks_count;
	int32_t r_blocks_count;
	int32_t free_blocks_count;
	int32_t free_inodes_count;
	int32_t first_data_block;
	int32_t log_block_size;
	int32_t log_frag_size;
	int32_t blocks_per_group;
	int32_t frags_per_group;
	int32_t inodes_per_group;
	int32_t mtime;
	int32_t wtime;
	int16_t mnt_count;
	int16_t max_mnt_count;
	int16_t magic;
	int16_t state;
	int16_t errors;
	int16_t minor_rev_level;
	int32_t lastcheck;
	int32_t checkinterval;
	int32_t creator_os;
	int32_t rev_level;
	int16_t def_resuid;
	int16_t def_resgid;

	// EXT2_DYNAMIC_REV
	int32_t first_ino;
	int16_t inode_size;
	int16_t block_group_nr;
	int32_t feature_compat;
	int32_t feature_incompat;
	int32_t feature_ro_compat;
	uint8_t uuid[16];
	char    volume_name[16];
	char    last_mounted[64];
	int32_t algorithm_usage_bitmap;

	// EXT2_COMPAT_PREALLOC
	uint8_t  prealloc_blocks;
	uint8_t  prealloc_dir_blocks;
	uint16_t padding1;

	// EXT2_FEATURE_COMPAT_HAS_JOURNAL
	uint8_t  journal_uuid[16];
	uint32_t journal_inum;
	uint32_t journal_dev;
	uint32_t last_orphan;
	uint32_t hash_seed[4];
	uint8_t  def_hash_version;
	uint8_t  reserved_char_pad;
	uint16_t reserved_word_pad;
	int32_t  default_mount_opts;
 	int32_t  first_meta_bg;
	uint32_t reserved[190];
} __attribute__((__packed__)) super_block_t;

typedef
struct group_desc_t
{
	int32_t  block_bitmap;
	int32_t  inode_bitmap;
	int32_t  inode_table;
	int16_t  free_blocks_count;
	int16_t  free_inodes_count;
	int16_t  used_dirs_count;
	uint16_t pad;
	int32_t  reserved[3];
} __attribute__((__packed__)) group_desc_t;

typedef
struct inode_t
{
	int16_t mode;
	int16_t uid;
	int32_t size;
	int32_t atime;
	int32_t ctime;
	int32_t mtime;
	int32_t dtime;
	int16_t gid;
	int16_t links_count;
	int32_t blocks;
	int32_t flags;

	union {
		struct {
			int32_t reserved1;
		} linux1;

		struct {
			int32_t translator;
		} hurd1;

		struct {
			int32_t reserved1;
		} masix1;
	} osd1;

	int32_t block[N_BLOCKS];
	int32_t generation;
	int32_t file_acl;
	int32_t dir_acl;
	int32_t faddr;

	union {
		struct {
			uint8_t  frag;
			uint8_t  fsize;
			uint16_t pad1;
			int16_t  uid_high;
			int16_t  gid_high;
			uint32_t reserved2;
		} linux2;

		struct {
			uint8_t frag;
			uint8_t fsize;
			int16_t mode_high;
			int16_t uid_high;
			int16_t gid_high;
			int32_t author;
		} hurd2;

		struct {
			uint8_t  frag;
			uint8_t  fsize;
			uint16_t pad1;
			uint32_t reserved2[2];
		} masix2;
	} osd2;
} __attribute__((__packed__)) inode_t;

typedef
struct dir_entry_t {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  filetype;
	char     name[NAME_LEN];
} __attribute__ ((__packed__)) dir_entry_t;

static
bool
recognize (FILE* disk)
{
	fseeko(disk, 1024, SEEK_CUR);

	super_block_t super_block;
	fread(&super_block, sizeof(super_block), 1, disk);

	if (super_block.magic != MAGIC ||
	    (super_block.feature_compat & FEATURE_COMPAT_HAS_JOURNAL) ||
	    (super_block.feature_ro_compat & ~EXT2_FEATURE_RO_COMPAT_SUPP) ||
	    (super_block.feature_incompat & ~EXT2_FEATURE_INCOMPAT_SUPP)) {
		return false;
	}

	return true;
}

typedef
struct file_system_t {
	super_block_t super_block;

	int32_t block_size;
	int32_t inodes_per_group;
	int16_t inode_size;

	struct {
		size_t length;
		group_desc_t* items;
	} group_descs;
} file_system_t;

static
file_system_t*
new_file_system (void)
{
	return memset(malloc(sizeof(file_system_t)), 0, sizeof(file_system_t));
}

static
void
destroy_file_system (file_system_t* file_system)
{
	if (file_system->group_descs.items) {
		free(file_system->group_descs.items);
	}
}

static
bool
read_inode (FILE* disk, file_system_t* file_system, uint32_t number, inode_t* out)
{
	return false;
}

static
bool
smash (FILE* disk, FILE* with, const char* path, hulk_flags_t mode)
{
	fseeko(disk, 1024, SEEK_SET);

	file_system_t* file_system = new_file_system();
	fread(&file_system->super_block, sizeof(file_system->super_block), 1, disk);

	if (file_system->super_block.state & ERROR_FS || file_system->super_block.state & ORPHAN_FS) {
		if (mode & HULK_VERBOSE) {
			fprintf(stderr, "hulk:ext2: the file system has errors or orphaned inodes being recovered\n");
		}

		goto error;
	}

	file_system->block_size       = BLOCK_SIZE(&file_system->super_block);
	file_system->inodes_per_group = INODES_PER_GROUP(&file_system->super_block);
	file_system->inode_size       = INODE_SIZE(&file_system->super_block);

	file_system->group_descs.length = file_system->super_block.blocks_count / BLOCKS_PER_GROUP(&file_system->super_block);
	file_system->group_descs.items  = malloc(sizeof(group_desc_t) * file_system->group_descs.length);
	fread(file_system->group_descs.items, sizeof(group_desc_t), file_system->group_descs.length, disk);

	inode_t root;
	read_inode(disk, file_system, ROOT_INO, &root);

ok:
	destroy_file_system(file_system);

	return true;

error:
	destroy_file_system(file_system);

	return false;
}

hulk_t ext2 = {
	.name = "ext2",

	.recognize = recognize,
	.smash     = smash
};

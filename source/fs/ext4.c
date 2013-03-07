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
#include <assert.h>

typedef
struct super_block_t
{
	int32_t inodes_count;
	int32_t blocks_count_lo;
	int32_t r_blocks_count_lo;
	int32_t free_blocks_count_lo;
	int32_t free_inodes_count;
	int32_t first_data_block;
	int32_t log_block_size;
	int32_t log_cluster_size;
	int32_t blocks_per_group;
	int32_t clusters_per_group;
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

	// EXT4_DYNAMIC_REV
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

	// EXT4_FEATURE_COMPAT_DIR_PREALLOC
	uint8_t prealloc_blocks;
	uint8_t prealloc_dir_blocks;
	int16_t reserved_gdt_blocks;

	// EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
	uint8_t journal_uuid[16];
	int32_t journal_inum;
	int32_t journal_dev;
	int32_t last_orphan;
	int32_t hash_seed[4];
	uint8_t def_hash_version;
	uint8_t jnl_backup_type;
	int16_t desc_size;
	int32_t default_mount_opts;
	int32_t first_meta_bg;
	int32_t mkfs_time;
	int32_t jnl_blocks[17];
	int32_t blocks_count_hi;
	int32_t r_blocks_count_hi;
	int32_t free_blocks_count_hi;
	int16_t min_extra_isize;
	int16_t want_extra_isize;
	int32_t flags;
	int16_t raid_stride;
	int16_t mmp_update_interval;
	int64_t mmp_block;
	int32_t raid_stripe_width;
	uint8_t log_groups_per_flex;
	uint8_t checksum_type;
	int16_t reserved_pad;
	int64_t kbytes_written;
	int32_t snapshot_inum;
	int32_t snapshot_id;
	int64_t snapshot_r_blocks_count;
	int32_t snapshot_list;
	int32_t error_count;
	int32_t first_error_time;
	int32_t first_error_ino;
	int64_t first_error_block;
	uint8_t first_error_func[32];
	int32_t first_error_line;
	int32_t last_error_time;
	int32_t last_error_ino;
	int32_t last_error_line;
	int64_t last_error_block;
	uint8_t last_error_func[32];
	uint8_t mount_opts[64];
	int32_t usr_quota_inum;
	int32_t grp_quota_inum;
	int32_t overhead_clusters;
	int32_t reserved[108];
	int32_t checksum;
} __attribute__((__packed__)) super_block_t;

typedef
struct group_desc_t
{
	int32_t  block_bitmap_lo;
	int32_t  inode_bitmap_lo;
	int32_t  inode_table_lo;
	int16_t  free_blocks_count_lo;
	int16_t  free_inodes_count_lo;
	int16_t  used_dirs_count_lo;
	int16_t  flags;
	int32_t  exclude_bitmap_lo;
	int16_t  block_bitmap_csum_lo;
	int16_t  inode_bitmap_csum_lo;
	int16_t  itable_unused_lo;
	int16_t  checksum;
	int32_t  block_bitmap_hi;
	int32_t  inode_bitmap_hi;
	int32_t  inode_table_hi;
	int16_t  free_blocks_count_hi;
	int16_t  free_inodes_count_hi;
	int16_t  used_dirs_count_hi;
	int16_t  itable_unused_hi;
	int32_t  exclude_bitmap_hi;
	int16_t  block_bitmap_csum_hi;
	int16_t  inode_bitmap_csum_hi;
	uint32_t reserved;
} __attribute__((__packed__)) group_desc_t;

typedef
struct inode_t {
	int16_t mode;
	int16_t uid;
	int32_t size_lo;
	int32_t atime;
	int32_t ctime;
	int32_t mtime;
	int32_t dtime;
	int16_t gid;
	int16_t links_count;
	int32_t blocks_lo;
	int32_t flags;

	union {
		struct {
			int32_t version;
		} linux1;

		struct {
			uint32_t translator;
		} hurd1;

		struct {
			uint32_t reserved1;
		} masix1;
	} osd1;

	int32_t block[N_BLOCKS];
	int32_t generation;
	int32_t file_acl_lo;
	int32_t size_high;
	int32_t obso_faddr;

	union {
		struct {
			int16_t blocks_high;
			int16_t file_acl_high;
			int16_t uid_high;
			int16_t gid_high;
			int16_t checksum_lo;
			int16_t reserved;
		} linux2;

		struct {
			int16_t  reserved1;
			uint16_t mode_high;
			uint16_t uid_high;
			uint16_t gid_high;
			uint32_t author;
		} hurd2;

		struct {
			int16_t  reserved1;
			int16_t  file_acl_high;
			uint32_t reserved2[2];
		} masix2;
	} osd2;

	int16_t extra_isize;
	int16_t checksum_hi;
	int32_t ctime_extra;
	int32_t mtime_extra;
	int32_t atime_extra;
	int32_t crtime;
	int32_t crtime_extra;
	int32_t version_hi;
} __attribute__((__packed__)) inode_t;

typedef
struct dir_entry_t {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  filetype;
	char     name[NAME_LEN];
} __attribute__ ((__packed__)) dir_entry_t;

typedef
struct extent_t {
	uint32_t block;
	uint16_t len;
	uint16_t start_hi;
	uint32_t start_lo;
} __attribute__ ((__packed__)) extent_t;

typedef
struct extent_index_t {
    uint32_t block;
    uint32_t leaf_lo;
    uint16_t leaf_hi;
    uint16_t unused;
} __attribute__ ((__packed__)) extent_index_t;

typedef
struct extent_header_t {
	uint16_t magic;
	uint16_t entries;
	uint16_t max;
	uint16_t depth;
	uint32_t generation;
} __attribute__ ((__packed__)) extent_header_t;

#define IMPLEMENTATION
#define EXTENTS
#include "ext.c"
#undef EXTENTS
#undef IMPLEMENTATION

static
bool
recognize (FILE* disk)
{
	fseeko(disk, 1024, SEEK_CUR);

	super_block_t super_block;
	fread(&super_block, sizeof(super_block), 1, disk);

	if (super_block.magic != MAGIC ||
	    (super_block.feature_incompat & FEATURE_INCOMPAT_JOURNAL_DEV) ||
	    !(super_block.feature_ro_compat & EXT3_FEATURE_RO_COMPAT_SUPP) ||
	    !(super_block.feature_incompat & EXT3_FEATURE_INCOMPAT_SUPP)) {
		return false;
	}

	return true;
}

static
bool
smash (FILE* disk, FILE* with, const char* path, hulk_flags_t mode)
{
	fseeko(disk, 1024, SEEK_SET);

	file_system_t* fs = new_file_system(disk);
	fread(&fs->super_block, sizeof(fs->super_block), 1, disk);

	if (fs->super_block.state & ERROR_FS || fs->super_block.state & ORPHAN_FS) {
		if (mode & HULK_VERBOSE) {
			fprintf(stderr, "hulk:ext4: the file system has errors or orphaned inodes being recovered\n");
		}

		goto error;
	}

	fs->block_size       = BLOCK_SIZE(&fs->super_block);
	fs->inodes_per_group = INODES_PER_GROUP(&fs->super_block);
	fs->inode_size       = INODE_SIZE(&fs->super_block);

	fs->group_descs.length = fs->super_block.blocks_count_lo / BLOCKS_PER_GROUP(&fs->super_block);
	fs->group_descs.items  = malloc(sizeof(group_desc_t) * fs->group_descs.length);
	fread(fs->group_descs.items, sizeof(group_desc_t), fs->group_descs.length, disk);

	read_inode(fs, ROOT_INO, &fs->root);

ok:
	destroy_file_system(fs);

	return true;

error:
	destroy_file_system(fs);

	return false;
}

hulk_t ext4 = {
	.name = "ext4",

	.recognize = recognize,
	.smash     = smash
};

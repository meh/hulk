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

#include <stdint.h>

#define MAGIC ((int16_t) 0xEF53)

#define VALID_FS  0x0001
#define ERROR_FS  0x0002
#define ORPHAN_FS 0x0004

// Special Inode Numbers
#define BAD_INO            1
#define ROOT_INO           2
#define ACL_IDX_INO        3
#define ACL_DATA_INO       4
#define BOOT_LOADER_INO    5
#define UNDEL_DIR_INO      6
#define GOOD_OLD_FIRST_INO 11

// Block Size Management
#define MIN_BLOCK_SIZE     1024
#define MAX_BLOCK_SIZE     4096
#define MIN_BLOCK_LOG_SIZE 10

// Fragment Sizes
#define MIN_FRAG_SIZE     1024
#define MAX_FRAG_SIZE     4096
#define MIN_FRAG_LOG_SIZE 10

#define FRAG_SIZE(s)       (MIN_FRAG_SIZE << (s)->log_frag_size)
#define FRAGS_PER_BLOCK(s) (BLOCK_SIZE(s) / FRAG_SIZE(s))

// Block Group Macros
# define BLOCKS_PER_GROUP(s) ((s)->blocks_per_group)
# define DESC_PER_BLOCK(s)   (BLOCK_SIZE(s) / sizeof(GROUP_DESC))
# define INODES_PER_GROUP(s) ((s)->inodes_per_group)
# define INODES_PER_BLOCK(s) (BLOCK_SIZE(s) / sizeof(INODE))
# define BLOCK_SIZE_BITS(s)  ((s)->log_block_size + 10)

// Constants relative to the data blocks
#define NDIR_BLOCKS 12
#define IND_BLOCK   NDIR_BLOCKS
#define DIND_BLOCK  (IND_BLOCK + 1)
#define TIND_BLOCK  (DIND_BLOCK + 1)
#define N_BLOCKS    (TIND_BLOCK + 1)

// Inode flags
#define SECRM_FL     0x00000001
#define UNRM_FL      0x00000002
#define COMPR_FL     0x00000004
#define SYNC_FL      0x00000008
#define IMMUTABLE_FL 0x00000010
#define APPEND_FL    0x00000020
#define NODUMP_FL    0x00000040
#define NOATIME_FL   0x00000080

// Reserved for compression usage...
#define DIRTY_FL    0x00000100
#define COMPRBLK_FL 0x00000200
#define NOCOMP_FL   0x00000400
#define ECOMPR_FL   0x00000800

// End compression flags --- maybe not all used
#define BTREE_FL        0x00001000
#define IMAGIC_FL       0x00002000
#define JOURNAL_DATA_FL 0x00004000
#define NOTAIL_FL       0x00008000
#define DIRSYNC_FL      0x00010000
#define TOPDIR_FL       0x00020000
#define HUGE_FILE_FL    0x00040000
#define EXTENTS_FL      0x00080000
#define RESERVED_FL     0x80000000

#define FL_USER_VISIBLE    0x00001FFF
#define FL_USER_MODIFIABLE 0x000000FF

// Codes for operating systems
#define OS_LINUX   0
#define OS_HURD    1
#define OS_MASIX   2
#define OS_FREEBSD 3
#define OS_LITES   4

// Revision levels
#define GOOD_OLD_REV 0
#define DYNAMIC_REV  1

#define CURRENT_REV  GOOD_OLD_REV
#define MAX_SUPP_REV DYNAMIC_REV

#define GOOD_OLD_INODE_SIZE 128

// Default values for user and/or group using reserved blocks
#define DEF_RESUID 0
#define DEF_RESGID 0

// Structure of a directory entry
#define NAME_LEN 255

#define BLOCK_SIZE(s)     (MIN_BLOCK_SIZE << (s)->log_block_size)
#define ADDR_PER_BLOCK(s) (BLOCK_SIZE(s) / sizeof(uint16_t))

#define INODE_SIZE(s) \
	(((s)->rev_level == GOOD_OLD_REV) ? \
		GOOD_OLD_INODE_SIZE : (s)->inode_size)

#define FIRST_INO(s) \
	(((s)->rev_level == GOOD_OLD_REV) ? \
		GOOD_OLD_FIRST_INO : (s)->first_ino)

#define EXT_MAGIC ((uint16_t) 0xF30A)
#define get_extent_header(i) \
	((extent_header_t*) (i)->block)

#define EXT_FIRST_EXTENT(header) \
	((extent_t*) (((char*) (header)) + sizeof(extent_header_t)))

#define EXT_FIRST_INDEX(header) \
	((extent_index_t*) (((char*) (header)) + sizeof(extent_header_t)))

#define INODE_HAS_EXTENT(i) \
	((i)->flags & EXTENTS_FL)

#define FEATURE_COMPAT_DIR_PREALLOC       0x0001
#define FEATURE_COMPAT_IMAGIC_INODES      0x0002
#define FEATURE_COMPAT_HAS_JOURNAL        0x0004
#define FEATURE_COMPAT_EXT_ATTR           0x0008
#define FEATURE_COMPAT_RESIZE_INODE       0x0010
#define FEATURE_COMPAT_DIR_INDEX          0x0020
#define FEATURE_RO_COMPAT_SPARSE_SUPER    0x0001
#define FEATURE_RO_COMPAT_LARGE_FILE      0x0002
#define FEATURE_RO_COMPAT_BTREE_DIR       0x0004
#define FEATURE_RO_COMPAT_HUGE_FILE       0x0008
#define FEATURE_RO_COMPAT_GDT_CSUM        0x0010
#define FEATURE_RO_COMPAT_DIR_NLINK       0x0020
#define FEATURE_RO_COMPAT_EXTRA_ISIZE     0x0040
#define FEATURE_RO_COMPAT_QUOTA           0x0100
#define FEATURE_RO_COMPAT_BIGALLOC        0x0200
#define FEATURE_RO_COMPAT_METADATA_CSUM   0x0400
#define FEATURE_INCOMPAT_COMPRESSION      0x0001
#define FEATURE_INCOMPAT_FILETYPE         0x0002
#define FEATURE_INCOMPAT_RECOVER          0x0004
#define FEATURE_INCOMPAT_JOURNAL_DEV      0x0008
#define FEATURE_INCOMPAT_META_BG          0x0010
#define FEATURE_INCOMPAT_EXTENTS          0x0040
#define FEATURE_INCOMPAT_64BIT            0x0080
#define FEATURE_INCOMPAT_MMP              0x0100
#define FEATURE_INCOMPAT_FLEX_BG          0x0200
#define FEATURE_INCOMPAT_EA_INODE         0x0400
#define FEATURE_INCOMPAT_DIRDATA          0x1000
#define FEATURE_INCOMPAT_BG_USE_META_CSUM 0x2000
#define FEATURE_INCOMPAT_LARGEDIR         0x4000
#define FEATURE_INCOMPAT_INLINEDATA       0x8000

#define EXT2_FEATURE_COMPAT_SUPP \
	FEATURE_COMPAT_EXT_ATTR

#define EXT2_FEATURE_INCOMPAT_SUPP ( \
	FEATURE_INCOMPAT_FILETYPE | \
	FEATURE_INCOMPAT_META_BG \
)

#define EXT2_FEATURE_RO_COMPAT_SUPP ( \
	FEATURE_RO_COMPAT_SPARSE_SUPER | \
	FEATURE_RO_COMPAT_LARGE_FILE | \
	FEATURE_RO_COMPAT_BTREE_DIR \
)

#define EXT3_FEATURE_COMPAT_SUPP \
	FEATURE_COMPAT_EXT_ATTR

#define EXT3_FEATURE_INCOMPAT_SUPP ( \
	FEATURE_INCOMPAT_FILETYPE | \
	FEATURE_INCOMPAT_RECOVER | \
	FEATURE_INCOMPAT_META_BG \
)

#define EXT3_FEATURE_RO_COMPAT_SUPP ( \
	FEATURE_RO_COMPAT_SPARSE_SUPER | \
	FEATURE_RO_COMPAT_LARGE_FILE | \
	FEATURE_RO_COMPAT_BTREE_DIR \
)

#define EXT4_FEATURE_COMPAT_SUPP \
	FEATURE_COMPAT_EXT_ATTR

#define EXT4_FEATURE_INCOMPAT_SUPP ( \
	FEATURE_INCOMPAT_FILETYPE | \
	FEATURE_INCOMPAT_RECOVER | \
	FEATURE_INCOMPAT_META_BG | \
	FEATURE_INCOMPAT_EXTENTS | \
	FEATURE_INCOMPAT_64BIT | \
	FEATURE_INCOMPAT_FLEX_BG | \
	FEATURE_INCOMPAT_MMP \
)

#define EXT4_FEATURE_RO_COMPAT_SUPP ( \
	FEATURE_RO_COMPAT_SPARSE_SUPER| \
	FEATURE_RO_COMPAT_LARGE_FILE| \
	FEATURE_RO_COMPAT_GDT_CSUM| \
	FEATURE_RO_COMPAT_DIR_NLINK | \
	FEATURE_RO_COMPAT_EXTRA_ISIZE | \
	FEATURE_RO_COMPAT_BTREE_DIR | \
	FEATURE_RO_COMPAT_HUGE_FILE | \
	FEATURE_RO_COMPAT_BIGALLOC | \
	FEATURE_RO_COMPAT_METADATA_CSUM | \
	FEATURE_RO_COMPAT_QUOTA \
)

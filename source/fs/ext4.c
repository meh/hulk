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

#include <stddef.h>
#include <stdint.h>

typedef struct superblock_t {
	int32_t inodes_count;
	int32_t blocks_count_lo;
	int32_t r_blocks_count_lo;
	int32_t free_blocks_count_lo;
	int32_t free_inodes_count;
	int32_t first_data_block;
	int32_t log_block_size;
	int32_t log_cluster_size;
	int32_t blocks_per_group;
	int32_t frags_per_group;
	int32_t inodes_per_group;
	int32_t mtime;
	int32_t wtime;
	int16_t mnt_count;
	int16_t max_mnt_count;
	int16_t magic; // 0xEF53
	int16_t state;
	int16_t errors;
	int16_t minor_rev_level;
	int32_t lastcheck;
	int32_t checkinterval;
	int32_t creator_os;
	int32_t rev_level;
	int16_t def_resuid;
	int16_t def_resgid;
} superblock_t;

static
bool
recognize (FILE* output)
{
	off_t beginning = ftello(output);
	bool  result    = false;

	fseeko(output, 1024, SEEK_CUR);
	fseeko(output, offsetof(superblock_t, magic), SEEK_CUR);

	int16_t magic = 0;
	fread(&magic, sizeof(magic), 1, output);

	if (magic == (int16_t) 0xEF53) {
		result = true;
	}

	fseeko(output, beginning, SEEK_SET);

	return result;
}

static
bool
smash (FILE* output, FILE* input, const char* path, bool only_date)
{
	return false;
}

hulk_t ext4 = {
	.recognize = recognize,
	.smash     = smash
};

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

#ifdef IMPLEMENTATION

typedef
struct file_system_t {
	FILE* disk;

	super_block_t super_block;
	inode_t       root;

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
new_file_system (FILE* disk)
{
	file_system_t* self = malloc(sizeof(file_system_t));

	memset(self, 0, sizeof(file_system_t));
	self->disk = disk;

	return self;
}

static
void
destroy_file_system (file_system_t* self)
{
	if (self->group_descs.items) {
		free(self->group_descs.items);
	}

	free(self);
}

typedef
struct file_t {
	file_system_t* fs;
	inode_t*       inode;
	uint32_t       inode_number;

	char* name;
	off_t size;
} file_t;

static
file_t*
new_file (file_system_t* fs)
{
	file_t* self = malloc(sizeof(file_t));

	memset(self, 0, sizeof(file_t));

	self->fs = fs;

	return self;
}

static
void
destroy_file (file_t* self)
{
	if (self->name) {
		free(self->name);
	}

	free(self);
}

static
bool
read_block (file_system_t* self, off_t number, void* out)
{
	fseeko(self->disk, number * self->block_size, SEEK_SET);
	fread(out, self->block_size, 1, self->disk);

	return true;
}

static
bool
read_inode (file_system_t* self, uint32_t number, inode_t* out)
{
	uint32_t group        = (number - 1) / self->inodes_per_group;
	uint32_t index        = ((number -1) % self->inodes_per_group) * self->inode_size;
	off_t    block_number = self->group_descs.items[group].inode_table_lo + (index / self->block_size);

	uint8_t block[self->block_size];
	read_block(self, block_number, block);

	memcpy(out, block + (index % self->block_size), sizeof(inode_t));

	return false;
}

#ifdef EXTENTS
static inline
uint64_t
extent_to_block (extent_t* extent)
{
	uint64_t block;

	block  = (uint64_t) extent->start_lo;
	block |= ((uint64_t) extent->start_hi << 31) << 1;

	return block;
}

static inline
uint64_t
extent_index_to_block (extent_index_t* index)
{
	uint64_t block;

	block  = (uint64_t) index->leaf_lo;
	block |= ((uint64_t) index->leaf_hi << 31) << 1;

	return block;
}

static
off_t
extent_binary_search (file_system_t* self, extent_header_t* header, off_t number)
{
	if (header->magic != EXT_MAGIC) {
		return 0;
	}

	extent_t* extent         = EXT_FIRST_EXTENT(header);
	off_t     physical_block = 0;

	if (header->depth == 0) {
		for (uint16_t i = 0; i < header->entries; i++, extent++) {
			if (number >= extent->block && number < (extent->block + extent->len)) {
				physical_block  = extent_to_block(extent) + number;
				physical_block -= extent->block;

				return physical_block;
			}
		}
		
		return 0;
	}

	extent_index_t* index = EXT_FIRST_INDEX(header);
	for (uint16_t i = 0; i < header->entries; i++) {
		if (number >= index->block) {
			uint8_t buffer[self->block_size];
			read_block(self, extent_index_to_block(index), buffer);

			return extent_binary_search(self, (extent_header_t*) buffer, number);
		}
	}

	return physical_block;
}
#endif

static
bool
read_data (file_system_t* self, inode_t* inode, off_t number, void* out)
{
	uint32_t block = 0;

	if (INODE_HAS_EXTENT(inode)) {
		#ifdef EXTENTS
		block = extent_binary_search(self, get_extent_header(inode), number);
		#endif

		goto end;
	}
	else {
		if (number < NDIR_BLOCKS) {
			block = inode->block[number];

			goto end;
		}

		uint32_t size     = self->block_size / sizeof(uint32_t);
		uint32_t ind_last = size + NDIR_BLOCKS;
		uint32_t ind_buffer[size];
		if (number < ind_last) {
			block = inode->block[IND_BLOCK];
			read_block(self, block, ind_buffer);

			number -= NDIR_BLOCKS;
			block   = ind_buffer[number];

			goto end;
		}

		uint32_t dind_last = (size * size) + ind_last;
		uint32_t dind_buffer[size];
		if (number >= ind_last && number < dind_last) {
			block = inode->block[DIND_BLOCK];
			read_block(self, block, dind_buffer);

			uint32_t tmp = number - ind_last;

			block = dind_buffer[tmp / size];
			read_block(self, block, ind_buffer);

			number = tmp % size;
			block  = ind_buffer[number];

			goto end;
		}

		uint32_t tind_buffer[size];
		if (number >= dind_last) {
			block = inode->block[TIND_BLOCK];
			read_block(self, block, tind_buffer);

			uint32_t tmp = number - dind_last;

			block = tind_buffer[tmp / (size * size)];
			read_block(self, block, dind_buffer);

			block  = tmp / size;
			number = tmp % size;
			block  = dind_buffer[block];
			read_block(self, block, ind_buffer);

			block = ind_buffer[number];

			goto end;
		}

		assert(false);
	}

end:
	if (block == 0) {
		return false;
	}

	return read_block(self, block, out);
}

#endif

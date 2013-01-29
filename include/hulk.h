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

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#	error "HULK NO LIKE BIG ENDIANS"
#endif

typedef int hulk_flags_t;

#define HULK_VERBOSE   0x1
#define HULK_ONLY_DATE 0x2
#define HULK_REMOVE    0x4

typedef
struct hulk_t
{
	const char* name;

	bool (*recognize)(FILE*);
	bool (*smash)(FILE*, FILE*, const char*, hulk_flags_t);
} hulk_t;

/***
 ** Recognize the file system and get the proper Hulk.
 */
hulk_t* hulk_recognize (FILE* device);

/***
 ** Destroy the file with the given content and the given times.
 */
bool hulk_smash (hulk_t* hulk, FILE* device, FILE* with, const char* path, hulk_flags_t mode);

/***
 ** Write to device the amount read from with, if with ends, it seeks to the beginning
 ** and continues.
 */
void hulk_write (FILE* device, FILE* with, size_t size);

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

typedef
struct hulk_t
{
	bool (*recognize)(FILE*);
	bool (*smash)(FILE*, FILE*, const char*, bool);

	void* private;
} hulk_t;

/***
 ** Recognize the file system and get the proper Hulk.
 */
hulk_t* hulk_recognize (FILE* output);

/***
 ** Destroy the file with the given content and the given times.
 */
bool hulk_smash (hulk_t* hulk, FILE* output, FILE* input, const char* path, bool only_date);

/***
 ** Write to output the amount read from input, if input ends, it seeks to the beginning
 ** and continues.
 */
void hulk_write (FILE* output, FILE* input, size_t size);

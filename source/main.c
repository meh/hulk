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
#include <mounts.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

extern hulk_t ext4;

hulk_t*
hulk_recognize (FILE* device)
{
	static hulk_t* hulks[] = { &ext4 };
	       hulk_t* result  = NULL;

	for (int i = 0; !result && i < sizeof(hulks) / sizeof(hulk_t*); i++) {
		off_t beginning = ftello(device);

		if (hulks[i]->recognize(device)) {
			result = hulks[i];
		}

		fseeko(device, beginning, SEEK_SET);
	}

	return result;
}

bool
hulk_smash (hulk_t* hulk, FILE* device, FILE* with, const char* path, hulk_flags_t mode)
{
	off_t beginning = ftello(device);
	bool  result    = hulk->smash(device, with, path, mode);

	fseeko(device, beginning, SEEK_SET);

	return result;
}

void
hulk_write (FILE* device, FILE* with, size_t size)
{
	size_t current = 0;
	size_t fill    = 0;
	char   buffer[512];

	while (current < size) {
		while (fill < 512) {
			size_t read = fread(buffer + fill, 1, 512 - fill, with);

			if (read < 512) {
				fseek(with, 0, SEEK_SET);
			}

			fill += read;
		}

		if (size - current < 512) {
			size_t written = fwrite(buffer, 1, size - current, device);

			current += written;
		}
		else {
			while (fill > 0 && current < size) {
				size_t written = fwrite(buffer + (512 - fill), 1, fill, device);

				current += written;
				fill    -= written;
			}
		}
	}
}

static
char*
device_for (const char* file)
{
	char*    result = NULL;
	char*    path   = realpath(file, NULL);
	FILE*    mounts = fopen("/proc/mounts", "r");
	mount_t* mount  = NULL;

	while ((mount = next_mount(mounts))) {
		if (strstr(path, mount->point) == path) {
			result = strdup(mount->device);
		}

		destroy_mount(mount);
	}

	fclose(mounts);
	free(path);

	return result;
}

static
char*
path_for (const char* file)
{
	char*    result = NULL;
	char*    path   = realpath(file, NULL);
	FILE*    mounts = fopen("/proc/mounts", "r");
	mount_t* mount  = NULL;

	while ((mount = next_mount(mounts)) && !result) {
		if (strstr(path, mount->point) == path) {
			result = strdup(path + strlen(mount->point));
		}

		destroy_mount(mount);
	}

	fclose(mounts);
	free(path);

	return result;
}

static
void
usage (void)
{
	fprintf(stderr,
		"Usage: hulk [OPTION]... FILE...\n"
		"\n"
		"  -w, --with       what to get the data from to smash with\n"
		"  -d, --device     the device where the files are (can be a simple file)\n"

		"  -V, --verbose    print verbose errors\n"
		"  -D, --date-only  scramble only the date\n"
		"  -R, --remove     remove the file too\n");

	exit(EXIT_FAILURE);
}

int
main (int argc, char* argv[])
{
	const char*        with_path   = "/dev/zero";
	const char*        device_path = NULL;
	      hulk_flags_t mode        = 0;

	while (true) {
		struct option options[] = {
			{"with",   required_argument, NULL, 'i'},
			{"device", required_argument, NULL, 'o'},

			{"verbose",   no_argument, NULL, 'V' },
			{"date-only", no_argument, NULL, 'D' },
			{"remove",    no_argument, NULL, 'R' }
		};

		int index = 0;
		int c     = getopt_long(argc, argv, "i:o:VDR", options, &index);

		if (c == -1) {
			break;
		}

		switch (c) {
			case 'i':
				with_path = optarg;
				break;

			case 'o':
				device_path = optarg;
				break;

			case 'V':
				mode |= HULK_VERBOSE;
				break;

			case 'D':
				mode |= HULK_ONLY_DATE;
				break;

			case 'R':
				mode |= HULK_REMOVE;
				break;

			default:
				usage();
		}
	}

	if (optind >= argc) {
		usage();
	}

	FILE* with = fopen(with_path, "r");
	if (with) {
		if (fgetc(with) != EOF) {
			fseek(with, -1, SEEK_CUR);
		}
		else {
			fprintf(stderr, "%s: Hulk cannot use!\n", with_path);

			return EXIT_FAILURE;
		}
	}
	else {
		fprintf(stderr, "%s: Hulk cannot use!\n", with_path);

		return EXIT_FAILURE;
	}

	for (int i = optind; i < argc; i++) {
		char* file_path = device_path ? strdup(argv[i])     : path_for(argv[i]);
		char* disk_path = device_path ? strdup(device_path) : device_for(argv[i]);

		FILE* disk = fopen(disk_path, "r+");

		if (disk) {
			hulk_t* hulk = hulk_recognize(disk);

			if (!hulk) {
				fprintf(stderr, "%s (%s): Hulk cannot smash that, is that adamantium or something?\n", file_path, disk_path);
			}
			else {
				if (!hulk_smash(hulk, disk, with, file_path, mode)) {
					fprintf(stderr, "%s (%s): Hulk cannot pick that up, I guess it was Thor's hammer!\n", file_path, disk_path);
				}
			}

			fclose(disk);
		}
		else {
			fprintf(stderr, "%s: Hulk cannot touch!\n", disk_path);
		}

		free(disk_path);
		free(file_path);
	}

	fclose(with);

	return EXIT_SUCCESS;
}

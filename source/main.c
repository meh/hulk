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
 * but WITHOUT ANY WARRANTY; inputout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along input hulk. If not, see <http://www.gnu.org/licenses/>.
 */

#include <hulk.h>
#include <mounts.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

extern hulk_t ext4;

hulk_t*
hulk_recognize (FILE* output)
{
	static hulk_t* hulks[] = { &ext4 };

	for (int i = 0; i < sizeof(hulks) / sizeof(hulk_t*); i++) {
		if (hulks[i]->recognize(output)) {
			return hulks[i];
		}
	}

	return NULL;
}

bool
hulk_smash (hulk_t* hulk, FILE* output, FILE* input, const char* path, bool only_date)
{
	return hulk->smash(output, input, path, only_date);
}

void
hulk_write (FILE* output, FILE* input, size_t size)
{
	size_t current = 0;
	size_t fill    = 0;
	char   buffer[512];

	while (current < size) {
		while (fill < 512) {
			size_t read = fread(buffer + fill, 1, 512 - fill, input);

			if (read < 512) {
				fseek(input, 0, SEEK_SET);
			}

			fill += read;
		}

		if (size - current < 512) {
			size_t written = fwrite(buffer, 1, size - current, output);

			current += written;
		}
		else {
			while (fill > 0 && current < size) {
				size_t written = fwrite(buffer + (512 - fill), 1, fill, output);

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
		"  -i, --input      what to get the data from to smash input\n"
		"  -o, --output     the device where the files are (can be a simple file)\n"
		"  -d, --date       scramble only the date\n");

	exit(EXIT_FAILURE);
}

int
main (int argc, char* argv[])
{
	const char* input_path  = "/dev/zero";
	const char* device_path = NULL;
	      int   only_date   = false;

	while (true) {
		struct option options[] = {
			{"date",   no_argument,       &only_date,  1 },

			{"input",  required_argument, NULL,       'i'},
			{"output", required_argument, NULL,       'o'},
		};

		int index = 0;
		int c     = getopt_long(argc, argv, "i:o:d", options, &index);

		if (c == -1) {
			break;
		}

		switch (c) {
			case 'i':
				input_path = optarg;
				break;

			case 'o':
				device_path = optarg;
				break;

			case 'd':
				only_date = true;
				break;

			default:
				usage();
		}
	}

	if (optind >= argc) {
		usage();
	}

	FILE* input = fopen(input_path, "r");
	if (input) {
		if (fgetc(input) != EOF) {
			fseek(input, -1, SEEK_CUR);
		}
		else {
			fprintf(stderr, "%s: Hulk cannot use!\n", input_path);

			return EXIT_FAILURE;
		}
	}
	else {
		fprintf(stderr, "%s: Hulk cannot use!\n", input_path);

		return EXIT_FAILURE;
	}

	for (int i = optind; i < argc; i++) {
		char* path   = device_path ? strdup(argv[i])     : path_for(argv[i]);
		char* device = device_path ? strdup(device_path) : device_for(argv[i]);

		FILE* output = fopen(device, "r+");

		if (output) {
			hulk_t* hulk = hulk_recognize(output);

			if (!hulk) {
				fprintf(stderr, "%s (%s): Hulk cannot smash that, is that adamantium or something?\n", path, device);
			}
			else {
				if (!hulk_smash(hulk, output, input, path, only_date)) {
					fprintf(stderr, "%s (%s): Hulk cannot pick that up, I guess it was Thor's hammer!\n", path, device);
				}
			}

			fclose(output);
		}
		else {
			fprintf(stderr, "%s: Hulk cannot touch!\n", device);
		}

		free(device);
		free(path);
	}

	fclose(input);

	return EXIT_SUCCESS;
}

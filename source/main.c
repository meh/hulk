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
#include <getopt.h>

extern hulk_t ext4;

hulk_t*
hulk_recognize (FILE* disk)
{
	static hulk_t* hulks[] = { &ext4 };

	for (int i = 0; i < sizeof(hulks) / sizeof(hulk_t*); i++) {
		if (hulks[i]->recognize(disk)) {
			return hulks[i];
		}
	}

	return NULL;
}

bool
hulk_smash (hulk_t* hulk, FILE* disk, const char* file, FILE* with, int times)
{
	return hulk->smash(disk, file, with, times);
}

typedef struct mount_t {
	char* device;
	char* point;
	char* type;
	char* options;

	int passes;
	int order;
} mount_t;

static
void
destroy_mount (mount_t* self)
{
	free(self->device);
	free(self->point);
	free(self->type);
	free(self->options);

	free(self);
}

static
mount_t*
next_mount (FILE* file)
{
	mount_t* mount = malloc(sizeof(mount_t));

	if (fscanf(file, "%as %as %as %as %d %d\n", &mount->device, &mount->point, &mount->type, &mount->options, &mount->passes, &mount->order) == EOF) {
		free(mount);

		return NULL;
	}

	return mount;
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
		"  -d, --device     the device where the files are (can be a simple file)\n");

	exit(EXIT_FAILURE);
}

int
main (int argc, char* argv[])
{
	const char* with_path   = "/dev/zero";
	const char* device_path = NULL;
	      int   times       = 1;

	while (true) {
		static struct option options[] = {
			{"with",   required_argument, 0, 'w'},
			{"device", required_argument, 0, 'd'},
			{"times",  required_argument, 0, 't'}
		};

		int index = 0;
		int c     = getopt_long(argc, argv, "w:d:t", options, &index);

		if (c == -1) {
			break;
		}

		switch (c) {
			case 'w':
				with_path = optarg;
				break;

			case 'd':
				device_path = optarg;
				break;

			case 't':
				times = atoi(optarg);
				break;

			default:
				usage();
		}
	}

	if (optind >= argc) {
		usage();
	}

	for (int i = optind; i < argc; i++) {
		char* file   = device_path ? strdup(argv[i])     : path_for(argv[i]);
		char* device = device_path ? strdup(device_path) : device_for(argv[i]);

		FILE* disk = fopen(device, "r+");
		FILE* with = fopen(with_path, "r");

		if (disk == NULL) {
			fprintf(stderr, "%s: Hulk cannot touch!\n", device);
		}

		if (with == NULL) {
			fprintf(stderr, "%s: Hulk cannot use!\n", with_path);
		}

		if (disk && with) {
			hulk_t* hulk = hulk_recognize(disk);

			if (!hulk) {
				fprintf(stderr, "%s (%s): Hulk cannot smash that, is that adamantium or something?\n", file, device);
			}
			else {
				if (!hulk_smash(hulk, disk, file, with, times)) {
					fprintf(stderr, "%s (%s): Hulk cannot pick that up, I guess it was Thor's hammer!\n", file, device);
				}
			}
		}

		if (disk) {
			fclose(disk);
		}

		if (with) {
			fclose(with);
		}

		free(device);
		free(file);
	}

	return EXIT_SUCCESS;
}

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

#include <mounts.h>

#include <stdlib.h>
#include <string.h>

mount_t*
new_mount (void)
{
	return memset(malloc(sizeof(mount_t)), 0, sizeof(mount_t));
}

void
destroy_mount (mount_t* self)
{
	if (self->device) {
		free(self->device);
	}
	
	if (self->point) {
		free(self->point);
	}

	if (self->type) {
		free(self->type);
	}

	if (self->options) {
		free(self->options);
	}

	free(self);
}

mount_t*
next_mount (FILE* mounts)
{
	mount_t* mount = new_mount();

	if (fscanf(mounts, "%as %as %as %as %d %d\n", &mount->device, &mount->point, &mount->type, &mount->options, &mount->passes, &mount->order) == EOF) {
		destroy_mount(mount);

		return NULL;
	}

	return mount;
}

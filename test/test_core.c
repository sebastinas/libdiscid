/* --------------------------------------------------------------------------

   MusicBrainz -- The Internet music metadatabase

   Copyright (C) 2013 Johannes Dewender

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>

#include <discid/discid.h>
#include "test.h"


int feature_consistent(const char *feature) {
	if (strcmp(feature, DISCID_FEATURE_STR_READ) == 0) {
		return discid_has_feature(DISCID_FEATURE_READ);
	} else if (strcmp(feature, DISCID_FEATURE_STR_MCN) == 0) {
		return discid_has_feature(DISCID_FEATURE_MCN);
	} else if (strcmp(feature, DISCID_FEATURE_STR_ISRC) == 0) {
		return discid_has_feature(DISCID_FEATURE_ISRC);
	} else {
		return 0;
	}
}

int main(int argc, char *argv[]) {
	DiscId *d;
	char *features[DISCID_FEATURE_LENGTH];
	char *feature;
	int i, found_features, invalid;
	int result;

	announce("discid_get_version_string");
	evaluate(strlen(discid_get_version_string()) > 0);

	announce("discid_get_feature_list");
	discid_get_feature_list(features);
	found_features = 0;
	invalid = 0;
	for (i = 0; i < DISCID_FEATURE_LENGTH; i++) {
		feature = features[i];
		if (feature) {
			found_features++;
			if (!feature_consistent(feature)) {
				invalid++;
			}
		}
	}
	evaluate(!invalid && found_features ==
			discid_has_feature(DISCID_FEATURE_READ)
			+ discid_has_feature(DISCID_FEATURE_MCN)
			+ discid_has_feature(DISCID_FEATURE_ISRC));

	announce("discid_get_default_device");
	/* this doesn't test much, but shouldn't fail completely
	 * TODO: make sure there is always something > 0 returned
	 */
	evaluate(strlen(discid_get_default_device()) >= 0);

	/* TODO
	 * test access with/without initialization doesn't fail
	 */

	announce("discid_new");
	d = discid_new();
	evaluate(d != NULL);

	announce("giving invalid device");
	result = discid_read(d, "invalid_device_name");
	evaluate(!result);
	announce("discid_get_error_msg");
	/* depending on result from invalid read
	 * If that fails, it still is only one failure.*/
	if (result) {
		evaluate(strlen(discid_get_error_msg(d)) == 0);
	} else {
		evaluate(strlen(discid_get_error_msg(d)) > 0);
	}

	/*
	announce("empty values");
	evaluate(discid_get_id(d) == NULL);
	*/
	/* TODO
	 * This needs implementation.
	 * Right now we get segmentation faults in debug builds (assert)
	 * and have to test for NULL in release builds.
	announce("empty values");
	evaluate(strlen(discid_get_id(d)) == 0
			&& strlen(discid_get_freedb_id(d)) == 0
			&& strlen(discid_get_submission_url(d)) == 0
			&& strlen(discid_get_mcn(d)) == 0
			&& discid_get_first_track_num(d) == 0
			&& discid_get_last_track_num(d) == 0
			&& discid_get_sectors(d) == 0);
			*/

	announce("discid_free");
	discid_free(d);
	evaluate(1); /* only segfaults etc. would "show" */
	
	return !test_result();
}

/* EOF */

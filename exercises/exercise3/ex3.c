/*
 * Copyright @2023 Christopher Roy. All rights reserved. Permission is
 * hereby granted to students registered for Unversity of Washgnton
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course. No other use, copying, distributon, or modification
 * is permitted wthout prior written consent. Copyrights for
 * third-party components of this work must be honored. Instructors
 * interested in reusing these course materials should contact the
 * author.
 *
 * Name: Christopher Roy
 * Email: cr0y@uw.edu
 */

#include "Point3d.h"
#include <stdio.h>
#include <stdlib.h>

// Standard main function
int main(int argc, char **argv) {
    Point3d* pt3d = Point3d_Allocate((int32_t) 1, (int32_t) 2, (int32_t) 3);
    if (pt3d->x != 1 || pt3d->y != 2 || pt3d->z != 3) {
        fprintf(stderr,
        "The pt3d was not allocated the correct initialized values");
        return EXIT_FAILURE;
    }

    Point3d* nlpt = NULL;
    Point3d_Scale(nlpt, 2);
    if (nlpt != NULL) {
        fprintf(stderr, "nlpt should not have worked as it is NULL");
        return EXIT_FAILURE;
    }

    Point3d_Scale(pt3d, 2);
    if (pt3d->x != 2 || pt3d->y != 4 || pt3d->z != 6) {
        fprintf(stderr,
        "The pt3d was not allocated the correct initialized values");
        return EXIT_FAILURE;
    }

    Point3d origpt = Point3d_GetOrigin();

    if (origpt.x != 0 || origpt.y != 0 || origpt.z != 0) {
        fprintf(stderr, "Origpt was not set to origin");
        return EXIT_FAILURE;
    }

    free(pt3d);
    return EXIT_SUCCESS;
}

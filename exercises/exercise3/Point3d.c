/*
 * Copyright @2023 Christopher Roy. All rights reserved. Permission is
 * hereby granted to students registered for Unversity of Washgnton
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course. No other use, copying, distributon, or modification
 * is permitted wthout prior written consent. Copyrights for
 * third-party components of this work must be honored. Instructors
 * interested in reusing these course materials should contact the
 =-* author.
 *
 * Name: Christopher Roy
 * Email: cr0y@uw.edu
 */

#include "Point3d.h"
#include <stdio.h>
#include <stdlib.h>

Point3d* Point3d_Allocate(int32_t xpt, int32_t ypt, int32_t zpt) {
    Point3d* pt3d = (Point3d*) malloc(sizeof(Point3d));
    if (pt3d != NULL) {
      pt3d->x = xpt;
      pt3d->y = ypt;
      pt3d->z = zpt;
    }
    return pt3d;
}

void Point3d_Scale(Point3d* pt3d, int32_t scaler) {
    if (pt3d != NULL) {
      pt3d->x = pt3d->x * scaler;
      pt3d->y = pt3d->y * scaler;
      pt3d->z = pt3d->z * scaler;
    }
}

Point3d Point3d_GetOrigin() {
    Point3d returner;
    returner.x = 0;
    returner.y = 0;
    returner.z = 0;
    return returner;
}

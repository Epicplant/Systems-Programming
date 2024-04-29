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

#ifndef POINT3D_H_
#define POINT3D_H_

#include <stdint.h>     // for uint64_t, etc.
///////////////////////////////////////////////////
//
typedef struct pt3 {
    int32_t x;
    int32_t y;
    int32_t z;
} Point3d;

// A function that mallocs space for a new
// Point3d object and assigns
// a x, y, and z value to it. It is the caller's
// responsibility to free the malloced space
// pointed to by the returned pointer. If
// malloc fails for whatever reason, pt3d is returned as NULL
Point3d* Point3d_Allocate(int32_t xpt, int32_t ypt, int32_t zpt);

// A functon that takes a ptr to a Point3d and proceeds
// to multiply its x, y, and z value by scaler. If the
// pointer for pt3d is null, nothing is scaled.
void Point3d_Scale(Point3d* pt3d, int32_t scaler);

// Creates a Point3d with all its fields set
// to 0. Does not require freeing from malloc.
Point3d Point3d_GetOrigin();

#endif  // POINT3D_H_

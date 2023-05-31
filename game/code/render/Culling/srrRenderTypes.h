#ifndef __SRR_RENDER_TYPES__
#define __SRR_RENDER_TYPES__

#include <render/culling/Vector3i.h>
#include <render/culling/Vector3f.h>
#include <render/culling/Bounds.h>
#include <render/culling/FloatFuncs.h>
//
// Values for unset/unused data fields
//
// Asserts on these values can be used 
// to detect erroneous game(object) states
//
// Any object shoulde be able to one of these
// values to initialize empty fields on finalizing 
// the creation process.
//

// Invalid Array Index
#define SRR_ERR_INDEX      -1
// Invalid World Space Coordinate
#define SRR_ERR_WS_COORD   -1.0f

#endif
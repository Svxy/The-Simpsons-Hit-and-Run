#ifndef SHAPE_CONSTANTS_H
#define SHAPE_CONSTANTS_H

#define LEAD_COLOR            1 // green
#define ACTIVE_COLOR       1 // white
#define ACTIVE_AFFECTED_COLOR 1  // purple
#define DORMANT_COLOR         1  // blue
#define HILITE_COLOR       1 // pale blue

#define P3D_BILLBOARD_QUAD_ID       0x040260
#define P3D_BILLBOARD_QUAD_GROUP_ID 0x040261
#define MAYA_LAMBERT_ID             1380729165
#define MAYA_PHONG_ID               1380993103
#define MAYA_LAYERED_SHADER_ID      1280922195

namespace TETreeLine
{

const float MIN_DISPLAY_SIZE=0.001f;

enum {
    WIREFRAME,
    WIREFRAME_SHADED,
    SMOOTH_SHADED,
    FLAT_SHADED,
    VERTICES,
    LAST_TOKEN
};

};  //namespace TETreeLine


#endif //SHAPE_CONSTANTS_H


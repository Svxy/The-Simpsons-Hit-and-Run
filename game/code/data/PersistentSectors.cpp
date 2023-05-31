#include <mission/charactersheet/charactersheetmanager.h>

// Auto generated with SectorTable. 
struct SectorMap
{
    unsigned short Sector;
    char CurIndex;
};

static const int NUM_SECTORS = NUM_PERSISTENT_SECTORS;

SectorMap sSectorMap[ NUM_SECTORS ] = {
{ 0x4d2d, 0 },    //l1r1.p3d
{ 0xedae, 0 },    //l1r2.p3d
{ 0x4b2b, 0 },    //l1r3.p3d
{ 0xc9f3, 0 },    //l1r4a.p3d
{ 0x6c70, 0 },    //l1r4b.p3d
{ 0xebaa, 0 },    //l1r6.p3d
{ 0x4917, 0 },    //l1r7.p3d
{ 0x4f25, 0 },    //l1z1.p3d
{ 0xefa6, 0 },    //l1z2.p3d
{ 0x4d23, 0 },    //l1z3.p3d
{ 0xedac, 0 },    //l1z4.p3d
{ 0xeda2, 0 },    //l1z6.p3d
{ 0x4b2f, 0 },    //l1z7.p3d
{ 0xdfa2, 0 },    //l2r1.p3d
{ 0x3f21, 0 },    //l2r2.p3d
{ 0xdfac, 0 },    //l2r3.p3d
{ 0x3f23, 0 },    //l2r4.p3d
{ 0xd19a, 0 },    //l2z1.p3d
{ 0xd279, 0 },    //l2z2.p3d
{ 0xd264, 0 },    //l2z3.p3d
{ 0x311b, 0 },    //l2z4.p3d
{ 0x6b2b, 0 },    //l3r1.p3d
{ 0x0da8, 0 },    //l3r2.p3d
{ 0x6d2d, 0 },    //l3r3.p3d
{ 0x0baa, 0 },    //l3r4.p3d
{ 0x6917, 0 },    //l3r5.p3d
{ 0x6d23, 0 },    //l3z1.p3d
{ 0x0fa0, 0 },    //l3z2.p3d
{ 0x6f25, 0 },    //l3z3.p3d
{ 0x0da2, 0 },    //l3z4.p3d
{ 0x6b2f, 0 },    //l3z5.p3d
{ 0x7590, 0 },    //l4r1.p3d
{ 0xd313, 0 },    //l4r2.p3d
{ 0x7596, 0 },    //l4r3.p3d
{ 0xe188, 0 },    //l4r4a.p3d
{ 0x3f0b, 0 },    //l4r4b.p3d
{ 0xd11f, 0 },    //l4r6.p3d
{ 0x7392, 0 },    //l4r7.p3d
{ 0x7398, 0 },    //l4z1.p3d
{ 0xd11b, 0 },    //l4z2.p3d
{ 0x739e, 0 },    //l4z3.p3d
{ 0x7279, 0 },    //l4z4.p3d
{ 0xdf07, 0 },    //l4z6.p3d
{ 0x719a, 0 },    //l4z7.p3d
{ 0x0d09, 0 },    //l5r1.p3d
{ 0xadaa, 0 },    //l5r2.p3d
{ 0x0b17, 0 },    //l5r3.p3d
{ 0xafa8, 0 },    //l5r4.p3d
{ 0x0f21, 0 },    //l5z1.p3d
{ 0xafa2, 0 },    //l5z2.p3d
{ 0x0d2f, 0 },    //l5z3.p3d
{ 0xb1a0, 0 },    //l5z4.p3d
{ 0x9fae, 0 },    //l6r1.p3d
{ 0xff2d, 0 },    //l6r2.p3d
{ 0x9fa8, 0 },    //l6r3.p3d
{ 0xfb17, 0 },    //l6r4.p3d
{ 0x9daa, 0 },    //l6r5.p3d
{ 0xa1a6, 0 },    //l6z1.p3d
{ 0x0125, 0 },    //l6z2.p3d
{ 0xa1a0, 0 },    //l6z3.p3d
{ 0xfd2f, 0 },    //l6z4.p3d
{ 0x9fa2, 0 },    //l6z5.p3d
{ 0x2b17, 0 },    //l7r1.p3d
{ 0xcdb4, 0 },    //l7r2.p3d
{ 0x2d09, 0 },    //l7r3.p3d
{ 0xe871, 0 },    //l7r4a.p3d
{ 0x88f2, 0 },    //l7r4b.p3d
{ 0xcfa8, 0 },    //l7r6.p3d
{ 0x2f2d, 0 },    //l7r7.p3d
{ 0x2d2f, 0 },    //l7z1.p3d
{ 0xcfac, 0 },    //l7z2.p3d
{ 0x2f21, 0 },    //l7z3.p3d
{ 0xd1a6, 0 },    //l7z4.p3d
{ 0xd1a0, 0 },    //l7z6.p3d
{ 0x3125, 0 } };  //l7z7.p3d

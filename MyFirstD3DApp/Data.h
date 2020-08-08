// Data.h

#if !defined(__Data_h__)
#define __Data_h__

// QUAD
const int QuadVtxListLen = 4;
extern Mesh::Vertex QuadVtxList[QuadVtxListLen];
const int QuadIdxListLen = 6;
extern WORD QuadIdxList[QuadIdxListLen][3];

// CUBE
const int CubeVtxListLen = 8;
extern Mesh::Vertex CubeVtxList[CubeVtxListLen];
const int CubeIdxListLen = 12;
extern WORD CubeIdxList[CubeIdxListLen][3];

// ICOSAHEDRON
const int IcosahedronVtxListLen = 12;
extern Mesh::Vertex IcosahedronVtxList[IcosahedronVtxListLen];
const int IcosahedronIdxListLen = 20;
extern WORD IcosahedronIdxList[IcosahedronIdxListLen][3];

// DODECAHEDRON
const int DodecahedronVtxListLen = 20;
extern Mesh::Vertex DodecahedronVtxList[DodecahedronVtxListLen];
const int DodecahedronIdxListLen = 36;
extern WORD DodecahedronIdxList[DodecahedronIdxListLen][3];

// ICOSIDODECAHEDRON
const int IcosidodecahedronVtxListLen = 30;
extern Mesh::Vertex IcosidodecahedronVtxList[IcosidodecahedronVtxListLen];
const int IcosidodecahedronIdxListLen = 56;
extern WORD IcosidodecahedronIdxList[IcosidodecahedronIdxListLen][3];

// SPHERE
const int SphereVtxListLen = 146;
extern Mesh::Vertex SphereVtxList[SphereVtxListLen];
const int SphereIdxListLen = 288;
extern WORD SphereIdxList[SphereIdxListLen][3];

#endif //__Data_h__

// endof Data.h
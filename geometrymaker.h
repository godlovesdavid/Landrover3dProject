#ifndef GEOMETRYMAKER_H
#define GEOMETRYMAKER_H

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "cvec.h"
#define PHI 1.618033988749894848204586834 //wow, such accuracy, much ratio, wow

//--------------------------------------------------------------------------------
// Helpers for creating some special geometries such as plane, cubes, and spheres
//--------------------------------------------------------------------------------


// A generic vertex structure containing position, normal, and texture information
// Used by make* functions to pass vertex information to the caller
struct GenericVertex
{
   Cvec3f pos;
   Cvec3f normal;
   Cvec2f tex;
   Cvec3f tangent, binormal;

   GenericVertex() {}
   GenericVertex(
      float x, float y, float z,
      float nx, float ny, float nz,
      float tu, float tv,
      float tx, float ty, float tz,
      float bx, float by, float bz)
      : pos(x, y, z), normal(nx, ny, nz), tex(tu, tv), tangent(tx, ty, tz), 
      binormal(bx, by, bz)
   {
   }
   GenericVertex(const GenericVertex& v)
   {
      *this = v;
   }

   GenericVertex& operator = (const GenericVertex& v)
   {
      pos = v.pos;
      normal = v.normal;
      tex = v.tex;
      tangent = v.tangent;
      binormal = v.binormal;
      return *this;
   }
};

inline void getPlaneVbIbLen(int& vbLen, int& ibLen)
{
   vbLen = 4;
   ibLen = 6;
}

template<typename VtxOutIter, typename IdxOutIter>
void makePlane(float size, VtxOutIter vtxIter, IdxOutIter idxIter)
{
   float h = size / 2.0;
   *vtxIter = GenericVertex(-h, 0, -h, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, -1);
   *(++vtxIter) = GenericVertex(-h, 0, h, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, -1);
   *(++vtxIter) = GenericVertex(h, 0, h, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, -1);
   *(++vtxIter) = GenericVertex(h, 0, -h, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, -1);
   *idxIter = 0;
   *(++idxIter) = 1;
   *(++idxIter) = 2;
   *(++idxIter) = 0;
   *(++idxIter) = 2;
   *(++idxIter) = 3;
}

inline void getCubeVbIbLen(int& vbLen, int& ibLen)
{
   vbLen = 24;
   ibLen = 36;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeCube(float size, VtxOutIter vtxIter, IdxOutIter idxIter)
{
   float h = size / 2.0;
#define DEFV(x, y, z, nx, ny, nz, tu, tv) { \
   *vtxIter = GenericVertex(x h, y h, z h, \
   nx, ny, nz, tu, tv, \
   tan[0], tan[1], tan[2], \
   bin[0], bin[1], bin[2]); \
   ++vtxIter; \
   }
   Cvec3f tan(0, 1, 0), bin(0, 0, 1);
   DEFV(+, -, -, 1, 0, 0, 0, 0); // facing +X
   DEFV(+, +, -, 1, 0, 0, 1, 0);
   DEFV(+, +, +, 1, 0, 0, 1, 1);
   DEFV(+, -, +, 1, 0, 0, 0, 1);

   tan = Cvec3f(0, 0, 1);
   bin = Cvec3f(0, 1, 0);
   DEFV(-, -, -, -1, 0, 0, 0, 0); // facing -X
   DEFV(-, -, +, -1, 0, 0, 1, 0);
   DEFV(-, +, +, -1, 0, 0, 1, 1);
   DEFV(-, +, -, -1, 0, 0, 0, 1);

   tan = Cvec3f(0, 0, 1);
   bin = Cvec3f(1, 0, 0);
   DEFV(-, +, -, 0, 1, 0, 0, 0); // facing +Y
   DEFV(-, +, +, 0, 1, 0, 1, 0);
   DEFV(+, +, +, 0, 1, 0, 1, 1);
   DEFV(+, +, -, 0, 1, 0, 0, 1);

   tan = Cvec3f(1, 0, 0);
   bin = Cvec3f(0, 0, 1);
   DEFV(-, -, -, 0, -1, 0, 0, 0); // facing -Y
   DEFV(+, -, -, 0, -1, 0, 1, 0);
   DEFV(+, -, +, 0, -1, 0, 1, 1);
   DEFV(-, -, +, 0, -1, 0, 0, 1);

   tan = Cvec3f(1, 0, 0);
   bin = Cvec3f(0, 1, 0);
   DEFV(-, -, +, 0, 0, 1, 0, 0); // facing +Z
   DEFV(+, -, +, 0, 0, 1, 1, 0);
   DEFV(+, +, +, 0, 0, 1, 1, 1);
   DEFV(-, +, +, 0, 0, 1, 0, 1);

   tan = Cvec3f(0, 1, 0);
   bin = Cvec3f(1, 0, 0);
   DEFV(-, -, -, 0, 0, -1, 0, 0); // facing -Z
   DEFV(-, +, -, 0, 0, -1, 1, 0);
   DEFV(+, +, -, 0, 0, -1, 1, 1);
   DEFV(+, -, -, 0, 0, -1, 0, 1);
#undef DEFV

   for (int v = 0; v < 24; v += 4)
   {
      *idxIter = v;
      *++idxIter = v + 1;
      *++idxIter = v + 2;
      *++idxIter = v;
      *++idxIter = v + 2;
      *++idxIter = v + 3;
      ++idxIter;
   }
}

inline void getSphereVbIbLen(int slices, int stacks, int& vbLen, int& ibLen)
{
   assert(slices > 1);
   assert(stacks >= 2);
   vbLen = (slices + 1) * (stacks + 1);
   ibLen = slices * stacks * 6;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeSphere(float radius, int slices, int stacks, VtxOutIter vtxIter, 
IdxOutIter idxIter)
{
   using namespace std;
   assert(slices > 1);
   assert(stacks >= 2);

   const double radPerSlice = 2 * CS175_PI / slices;
   const double radPerStack = CS175_PI / stacks;

   vector<double> longSin(slices + 1), longCos(slices + 1);
   vector<double> latSin(stacks + 1), latCos(stacks + 1);
   for (int i = 0; i < slices + 1; ++i)
   {
      longSin[i] = sin(radPerSlice * i);
      longCos[i] = cos(radPerSlice * i);
   }
   for (int i = 0; i < stacks + 1; ++i)
   {
      latSin[i] = sin(radPerStack * i);
      latCos[i] = cos(radPerStack * i);
   }

   for (int i = 0; i < slices + 1; ++i)
   {
      for (int j = 0; j < stacks + 1; ++j)
      {
         float x = longCos[i] * latSin[j];
         float y = longSin[i] * latSin[j];
         float z = latCos[j];

         Cvec3f n(x, y, z);
         Cvec3f t(-longSin[i], longCos[i], 0);
         Cvec3f b = cross(n, t);

         *vtxIter = GenericVertex(
            x * radius, y * radius, z * radius,
            x, y, z,
            1.0 / slices*i, 1.0 / stacks*j,
            t[0], t[1], t[2],
            b[0], b[1], b[2]);
         ++vtxIter;

         if (i < slices && j < stacks)
         {
            *idxIter = (stacks + 1) * i + j;
            *++idxIter = (stacks + 1) * i + j + 1;
            *++idxIter = (stacks + 1) * (i + 1) + j + 1;

            *++idxIter = (stacks + 1) * i + j;
            *++idxIter = (stacks + 1) * (i + 1) + j + 1;
            *++idxIter = (stacks + 1) * (i + 1) + j;
            ++idxIter;
         }
      }
   }
}

inline void getIcosVbIbLen(int& vbLen, int& ibLen)
{
   vbLen = 60;
   ibLen = 60;
}

/*
PURPOSE: make icosahedron.
*/
template<typename VtxOutIter, typename IdxOutIter>
void makeIcos(float size, VtxOutIter vtxIter, IdxOutIter idxIter)
{
   float s = size / 2;
   Cvec3f norm, tan, bin;
#define TRI(p0, p1, p2, t0, t1, t2) {\
   norm = cross((p0 - p1), (p1 - p2)).normalize(); \
   tan = (p0 - p1).normalize(); \
   bin = cross(norm, tan); \
   *vtxIter++ = GenericVertex(p0[0], p0[1], p0[2], \
   norm[0], norm[1], norm[2], t0[0], t0[1], \
   tan[0], tan[1], tan[2], \
   bin[0], bin[1], bin[2]); \
   \
   *vtxIter++ = GenericVertex(p1[0], p1[1], p1[2], \
   norm[0], norm[1], norm[2], t1[0], t1[1], \
   tan[0], tan[1], tan[2], \
   bin[0], bin[1], bin[2]); \
   \
   *vtxIter++ = GenericVertex(p2[0], p2[1], p2[2], \
   norm[0], norm[1], norm[2], t2[0], t2[1], \
   tan[0], tan[1], tan[2], \
   bin[0], bin[1], bin[2]); \
   }

   //make our twelve vertices
   Cvec3f verts[12];
   verts[0] = Cvec3f(1, 0, -PHI);
   verts[1] = Cvec3f(-PHI, -1, 0);
   verts[2] = Cvec3f(0, -PHI, -1);
   verts[3] = Cvec3f(0, -PHI, 1);
   verts[4] = Cvec3f(PHI, -1, 0);
   verts[5] = Cvec3f(1, 0, PHI);
   verts[6] = Cvec3f(PHI, 1, 0);
   verts[7] = Cvec3f(0, PHI, 1);
   verts[8] = Cvec3f(0, PHI, -1);
   verts[9] = Cvec3f(-PHI, 1, 0);
   verts[10] = Cvec3f(-1, 0, -PHI);
   verts[11] = Cvec3f(-1, 0, PHI);

   int indices[] =
   { 0, 4, 2, 0, 6, 4, 0, 8, 6, 0, 10, 8, 0, 2, 10, 1, 2, 3, 2, 4, 3,
   3, 4, 5, 4, 6, 5, 5, 6, 7, 6, 8, 7, 7, 8, 9, 8, 10, 9, 9, 10, 1,
   10, 2, 1, 1, 3, 11, 3, 5, 11, 5, 7, 11, 7, 9, 11, 9, 1, 11 };

   for (int i = 0; i < 20; i++)
   {
      Cvec3f p0 = verts[indices[3 * i]];
      Cvec3f p1 = verts[indices[3 * i + 1]];
      Cvec3f p2 = verts[indices[3 * i + 2]];
      /*Cvec2f t0(0, 0);
      Cvec2f t1(.5, 1);
      Cvec2f t2(1, 0);*/
      float offset = 0.43301270189; //(sqrt 3)/4
      Cvec2f t0(0, .5 - offset);
      Cvec2f t1(.5, .5 + offset);
      Cvec2f t2(1, .5 - offset);

      TRI(p0, p1, p2, t0, t1, t2);
   }

   for (int i = 0; i < 60; i++)
      *idxIter++ = i;

#undef TRI
}

void drawBezier(float*, int, int);

/* PURPOSE: randomly generate points based on the perlin noise technique.
   RECEIVES: points in float, number of octaves, and seed value
*/
void perlinNoise(float* points, int octaves, long seed)
{
   srand(seed);
   int n = (1 << octaves);
   for (int i = 0; i < n*n; i++)
      points[i] = 0;

   for (int o = 0; o < octaves; o++)
   {
      int delta = 1 << o;   //delta is 2^i
      float height = 32.0 / (1 << octaves) / delta;

      for (int i = 0; i < (1 << octaves); i += delta)
         for (int j = 0; j < (1 << octaves); j += delta)
         {
            points[n*i * 3 + 3 * j] = ((float)i) / n;
            points[n*i * 3 + 3 * j + 1] = ((float)j) / n;
            if (i != 0 && i != n - 1 && j != 0 && j != n - 1)
               points[n*i * 3 + 3 * j + 2] += fmod(rand(), height);
         }
   }
}

#endif



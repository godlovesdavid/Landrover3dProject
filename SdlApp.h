/* INCLUDES */
#ifndef _SdlApp_H_
#define _SdlApp_H_
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <stdexcept>
#if __GNUG__
#   include <tr1/memory>
#endif
#include "cvec.h"
#include "matrix4.h"
#include "geometrymaker.h"
#include "ppm.h"
#include "glsupport.h"
#include <iostream>
#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>

using namespace std;
//for string, vector, iostream, and other standard C++ stuff
using namespace tr1;

/*
This application uses SDL functions to make window and handle events.
*/
class SdlApp
{
private:
   bool running;
   SDL_Window* display;
   void keydown(const char * key);

public:
   SdlApp();
   int run();
   void handleEvent(SDL_Event* e);
   void clearCanvas();
   void draw();
};


/*
Shader state of a GL program.
It holds uniform variables and vertex attributes.
It also holds handle to a program, and attaches shaders to it.
*/
struct ShaderState
{
   GlProgram program;

   // Handles to uniform variables
   GLint h_uLight;
   GLint h_uProjMatrix;
   GLint h_uModelViewMatrix;
   GLint h_uNormalMatrix;
   GLint h_uTexUnit;

   // Handles to vertex attributes
   GLint h_aPosition;
   GLint h_aNormal;
   GLint h_aTexCoord;

   ShaderState(const char* vsfn, const char* fsfn, int numtextures)
   {
      readAndCompileShader(program, vsfn, fsfn);
      const GLuint h = program; // short hand

      // Retrieve handles to uniform variables
      h_uLight = safe_glGetUniformLocation(h, "uLight");
      h_uProjMatrix = safe_glGetUniformLocation(h, "uProjMatrix");
      h_uModelViewMatrix = safe_glGetUniformLocation(h, "uModelViewMatrix");
      h_uNormalMatrix = safe_glGetUniformLocation(h, "uNormalMatrix");
      h_uTexUnit = safe_glGetUniformLocation(h, "uTexUnit");

      // Retrieve handles to vertex attributes
      h_aPosition = safe_glGetAttribLocation(h, "aPosition");
      h_aNormal = safe_glGetAttribLocation(h, "aNormal");
      h_aTexCoord = safe_glGetAttribLocation(h, "aTexCoord");

      checkGlErrors();
   }
};


// --------- Geometry
// Macro used to obtain relative offset of a field within a struct
#define FIELD_OFFSET(StructType, field) &(((StructType *)0)->field)

/*
Geometry struct.
It holds the coordinates of vectices and textures in buffer objects
and draws them.
*/
struct Geometry
{
   GlBufferObject vbo, texVbo, ibo;
   int vboLen, iboLen;

   Geometry(GenericVertex *vtx, unsigned short *idx, int vboLen, int iboLen)
   {
      this->vboLen = vboLen;
      this->iboLen = iboLen;

      // create vertex buffer object
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GenericVertex)* vboLen, vtx,
         GL_STATIC_DRAW);

      //create index buffer object
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)* iboLen,
         idx, GL_STATIC_DRAW);

      //create texture buffer object
      glBindBuffer(GL_ARRAY_BUFFER, texVbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GenericVertex)* vboLen, vtx,
         GL_STATIC_DRAW);
   }

   /*
   PURPOSE: Draws the opengl objects.
   Uses what shader state specifies such as drawing a sphere, giving it 
   coordinates, and its texture.
   RECEIVES:   current shader state address
   */
   void draw(const ShaderState& CUR_SS)
   {
      // Enable the attributes used by our shader
      safe_glEnableVertexAttribArray(CUR_SS.h_aPosition);
      safe_glEnableVertexAttribArray(CUR_SS.h_aNormal);
      safe_glEnableVertexAttribArray(CUR_SS.h_aTexCoord);

      // bind vertex buffer object
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      safe_glVertexAttribPointer(CUR_SS.h_aPosition, 3, GL_FLOAT, GL_FALSE,
         sizeof(GenericVertex), FIELD_OFFSET(GenericVertex, pos));
      safe_glVertexAttribPointer(CUR_SS.h_aNormal, 3, GL_FLOAT, GL_FALSE,
         sizeof(GenericVertex), FIELD_OFFSET(GenericVertex, normal));

      //bind texture buffer object
      glBindBuffer(GL_ARRAY_BUFFER, texVbo);
      safe_glVertexAttribPointer(CUR_SS.h_aTexCoord, 2, GL_FLOAT, GL_FALSE,
         sizeof(GenericVertex), FIELD_OFFSET(GenericVertex, tex));

      // bind index buffer object
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

      // draw!
      glDrawElements(GL_TRIANGLES, iboLen, GL_UNSIGNED_SHORT, 0);

      // Disable the attributes used by our shader
      safe_glDisableVertexAttribArray(CUR_SS.h_aPosition);
      safe_glDisableVertexAttribArray(CUR_SS.h_aNormal);
      safe_glDisableVertexAttribArray(CUR_SS.h_aTexCoord);
   }
};
#endif

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "glsupport.h"

using namespace std;

void checkGlErrors()
{
   //ignore all errors, yolo
   return;
   const GLenum errCode = glGetError();

   //because glewInit was throwing an invalid enum error for some reason...
   if (errCode != GL_NO_ERROR && errCode != GL_INVALID_ENUM)
   {
      string error("GL Error: ");
      switch (errCode)
      {
      case GL_INVALID_ENUM:
         error += "GL_INVALID_ENUM";
         break;
      case GL_INVALID_VALUE:
         error += "GL_INVALID_VALUE";
         break;
      case GL_INVALID_OPERATION:
         error += "GL_INVALID_OPERATION";
         break;
      case GL_NO_ERROR:
         error += "GL_NO_ERROR";
         break;
      case GL_STACK_OVERFLOW:
         error += "GL_STACK_OVERFLOW";
         break;
      case GL_STACK_UNDERFLOW:
         error += "GL_STACK_UNDERFLOW";
         break;
      case GL_OUT_OF_MEMORY:
         error += "GL_OUT_OF_MEMORY";
         break;
      default:
         error += "unknown error lolz...";
      }
      cerr << error << endl;
      throw runtime_error(error);
   }
}

// Dump text file into a character vector, throws exception on error
static void readTextFile(const char *fn, vector<char>& data)
{
   // Sets ios::binary bit to prevent end of line translation, so that the
   // number of bytes we read equals file size
   ifstream ifs(fn, ios::binary);
   if (!ifs)
      throw runtime_error(string("Cannot open file ") + fn);

   // Sets bits to report IO error using exception
   ifs.exceptions(ios::eofbit | ios::failbit | ios::badbit);
   ifs.seekg(0, ios::end);
   size_t len = ifs.tellg();
   data.resize(len);
   ifs.seekg(0, ios::beg);
   ifs.read(&data[0], len);
}

/*static void printShaderInfoLog(GLuint shaderHandle, const string& fn){
  int length;
  glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
  char log[length];
  glGetShaderInfoLog(shaderHandle, length, &length, log);
  cerr <<"Shader log[" << fn << "]:" << endl << log << endl;
  }*/

/*static void printProgramInfoLog(GLuint programHandle, const string& fn){
  int length;
  glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &length);
  char log[length];
  glGetShaderInfoLog(programHandle, length, &length, log);
  cerr <<"Program log[" << fn << "]:" << endl << log << endl;
  }*/

void readAndCompileSingleShader(GLuint shaderHandle, const char *fn)
{
   vector<char> source;
   readTextFile(fn, source);
   const char *ptrs[] = { &source[0] };
   const GLint lens[] = { (GLint)source.size() };
   glShaderSource(shaderHandle, 1, ptrs, lens);   // load the shader sources
   glCompileShader(shaderHandle);
   //printShaderInfoLog(shaderHandle, fn);
   GLint compiled = 0;
   glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      throw runtime_error("fails to compile GL shader");
   }
}

void linkShader(GLuint programHandle, GLuint vs, GLuint fs)
{
   glAttachShader(programHandle, vs);
   glAttachShader(programHandle, fs);

   glLinkProgram(programHandle);

   glDetachShader(programHandle, vs);
   glDetachShader(programHandle, fs);

   GLint linked = 0;
   glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
   //printProgramInfoLog(programHandle, "linking");

   if (!linked)
      throw runtime_error("fails to link shaders");
}


void readAndCompileShader(GLuint programHandle, const char 
* vertexShaderFileName, const char * fragmentShaderFileName)
{
   GlShader vs(GL_VERTEX_SHADER);
   GlShader fs(GL_FRAGMENT_SHADER);

   readAndCompileSingleShader(vs, vertexShaderFileName);
   readAndCompileSingleShader(fs, fragmentShaderFileName);

   linkShader(programHandle, vs, fs);
}

/*********************************************
 * Project: CS116B Homework #3
 * Purpose: move up and down a land rover on a noise terrain.
 * Programmer: David Hsu, Sunny Le, Sean Papay
 **********************************************/
/* INCLUDES */
#include "SdlApp.h"

/* GLOBALS */
// shaders
vector<shared_ptr<ShaderState> > shaderStates;
const char * const SHADER_SETS[][2] =
{
   { "./shaders/ground.vshader", "./shaders/ground.fshader" },
   { "./shaders/roverbody.vshader", "./shaders/roverbody.fshader" }
};

// camera
const float FRUST_MIN_FOV = 60.0; // minimum field of view
const float FRUST_NEAR = -0.1;   // near plane
const float FRUST_FAR = -50.0;   // far plane
float frustFovY = FRUST_MIN_FOV; // y dir field of view
int windowWidth = 680;
int windowHeight = 480;
Matrix4 cameraposition = Matrix4::makeTranslation(Cvec3(0, 2, 10))
* Matrix4::makeXRotation(-22.8);

// objects
const char * OBJECT_TYPES[] =
{ "plane", "icosahedron" }; //0-> ground 1-> rover
Matrix4 objectpositions[] =
{ Matrix4::makeScale(Cvec3(20, 1, 20))
* Matrix4::makeTranslation(Cvec3(0, -2, 0)), Matrix4() };
//shade mode of each object corresponding to shader set number
const int OBJECT_SHADE_MODE[] = { 0, 1 };
shared_ptr<Geometry> objects[sizeof(OBJECT_TYPES) 
/ sizeof(OBJECT_TYPES[0])];

//mountains
float m1[1000], m2[1000], m3[1000];

// textures
const char * TEXTURE_PATHS[] =
{ "./crater.jpg", "./lander.jpg" };
shared_ptr<GlTexture> textures[sizeof(TEXTURE_PATHS) 
/ sizeof(TEXTURE_PATHS[0])];

// animation
float landery = 0;
float landerdy = 0;

/* PURPOSE: handle animation.
 */
void handleAnimation()
{
   if (landery <= 0 && landerdy <= 0)
   {
      landery = 0;
      landerdy = 0;
   }
   else
   {
      landery += landerdy;
      landerdy -= .0001;
   }
   objectpositions[1] = Matrix4::makeXRotation(-90)
      * Matrix4::makeTranslation(Cvec3(0, 0, landery));
}

/* PURPOSE: draw bezier patch given points.
RECEIVES: points, v stride, num of u points.
*/
void drawBezier(GLfloat* points, int r, int c)
{
   glMap2f(GL_MAP2_VERTEX_3, 0.0 /* uMin*/, 1.0 /* uMax*/,
      3 /* u stride*/, c /*num points u*/,
      0.0 /* vMin*/, 1.0 /* vMax*/, 3 * c /*v stride*/,
      r, points);

   glEnable(GL_MAP2_VERTEX_3);
   GLint k, j;

   //draw patch in gray
   glColor3f(.3, .3, .3);
   glMapGrid2f(50, 0.0, 1.0, 50, 0.0, 1.0);
   glEvalMesh2(GL_FILL, 0, 50, 0, 50);

   for (k = 0; k <= 8; k++)
   {
      glBegin(GL_LINE_STRIP);
      for (j = 0; j <= 40; j++)
         glEvalCoord2f(GLfloat(j) / 40.0, GLfloat(k) / 8.0);
      glEnd();

      glBegin(GL_LINE_STRIP);
      for (j = 0; j <= 40; j++)
         glEvalCoord2f(GLfloat(k) / 8.0, GLfloat(j) / 40.0);
      glEnd();
   }
   glFlush();
}

/* PURPOSE: Initialize the shader states by picking what files we want to use.
 */
void makeShaders()
{
   shaderStates.resize(sizeof(SHADER_SETS) / sizeof(SHADER_SETS[0]));
   for (int i = 0; i < sizeof(SHADER_SETS) / sizeof(SHADER_SETS[0]); i++)
      shaderStates[i].reset(
      new ShaderState(SHADER_SETS[i][0], SHADER_SETS[i][1],
      sizeof(TEXTURE_PATHS) / sizeof(TEXTURE_PATHS[0])));
}

/* PURPOSE: Initializes all the goemetry in the sdl window.
 */
void makeObjects()
{
   int ibLen, vbLen;
   vector<GenericVertex> vtx;
   vector<unsigned short> idx;
   for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++)
   {
      if (OBJECT_TYPES[i] == "cube")
         makeCube(1, (vtx = vector<GenericVertex>(vbLen = 24)).begin(), 
         (idx = vector<unsigned short>(ibLen = 36)).begin());
      else if (OBJECT_TYPES[i] == "plane")
         makePlane(1, (vtx = vector<GenericVertex>(vbLen = 4)).begin(), 
         (idx = vector<unsigned short>(ibLen = 6)).begin());
      else if (OBJECT_TYPES[i] == "sphere")
         makeSphere(1, 80, 80, 
         (vtx = vector<GenericVertex>(vbLen = 6561)).begin(), 
         (idx = vector<unsigned short>(ibLen = 38400)).begin());
      else if (OBJECT_TYPES[i] == "icosahedron")
         makeIcos(1, (vtx = vector<GenericVertex>(vbLen = 60)).begin(), 
         (idx = vector<unsigned short>(ibLen = 60)).begin());

      objects[i].reset(new Geometry(&vtx[0], &idx[0], vbLen, ibLen));
   }
}

/* PURPOSE: Initializes the texture.
 */
void makeTextures()
{
   GLenum activetex = 0x84C0;
   for (int i = 0; i < sizeof(TEXTURE_PATHS) / sizeof(TEXTURE_PATHS[0]); i++)
   {
      textures[i].reset(new GlTexture());
      vector<PackedPixel> pixData;
      SDL_Surface* newSurface = IMG_Load(TEXTURE_PATHS[i]); // read in image
      // convert from RGBA to RGB
      SDL_Surface* returnSurface = SDL_ConvertSurfaceFormat(newSurface,
         SDL_PIXELFORMAT_RGB24, 0);
      glActiveTexture(activetex++);
      glBindTexture(GL_TEXTURE_2D, *textures[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, returnSurface->w, 
      returnSurface->h, 0, GL_RGB,
         GL_UNSIGNED_BYTE, returnSurface->pixels);
      SDL_FreeSurface(newSurface);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   }
}

/* PURPOSE: Creates an sdl application.
 */
SdlApp::SdlApp()
{
   running = true;

   SDL_Init(SDL_INIT_EVERYTHING);

   // Turn on double buffering with a 24bit Z buffer.
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   display = SDL_CreateWindow("hw3", SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

   // Create our opengl context and attach it to our window
   SDL_GLContext maincontext = SDL_GL_CreateContext(display);

   // buffer swap syncronized with monitor's vertical refresh 
   SDL_GL_SetSwapInterval(1);

   //init more stuff
   glewInit();
   glClearDepth(0);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_GREATER);
   glEnable(GL_MULTISAMPLE); //enable oversampling

   makeShaders();
   makeObjects();
   makeTextures();
}

/* PURPOSE: Keyboard events for th sdl window.
 RECEIVES: an SDL event pointer.
 */
void SdlApp::handleEvent(SDL_Event* e)
{
   //quit
   if (e->type == SDL_QUIT)
      running = false;
   //keys
   if (e->key.keysym.sym == SDLK_SPACE && landery <= 0)
      landerdy = .015;
}

/* PURPOSE: clear framebuffer color & depth.
 */
void SdlApp::clearCanvas()
{
   SDL_GL_SwapWindow(display);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* PURPOSE: draw mountains.
*/
void drawMountains(const ShaderState& shaderstate)
{
   GLfloat matrix[16];

   //back mountain
   Matrix4 MVM = inv(cameraposition)  
   * Matrix4::makeTranslation(Cvec3(-5, -2, -5)) 
   * Matrix4::makeScale(Cvec3(12, 4, 12)) * Matrix4::makeXRotation(-90);
   MVM.writeToColumnMajorMatrix(matrix);
   safe_glUniformMatrix4fv(shaderstate.h_uModelViewMatrix, matrix);
   drawBezier(m1, 8, 8);

   //left mountain
   MVM = inv(cameraposition)  * Matrix4::makeTranslation(Cvec3(-16.5, -2, 0)) 
   * Matrix4::makeScale(Cvec3(12, 3, 12)) * Matrix4::makeYRotation(-30) 
   * Matrix4::makeXRotation(-90);
   MVM.writeToColumnMajorMatrix(matrix);
   safe_glUniformMatrix4fv(shaderstate.h_uModelViewMatrix, matrix);
   drawBezier(m2, 8, 8);

   //right mountain
   MVM = inv(cameraposition)  * Matrix4::makeTranslation(Cvec3(5, -2, 2)) 
   * Matrix4::makeScale(Cvec3(12, 3, 12)) * Matrix4::makeYRotation(0) 
   * Matrix4::makeXRotation(-90);
   MVM.writeToColumnMajorMatrix(matrix);
   safe_glUniformMatrix4fv(shaderstate.h_uModelViewMatrix, matrix);
   drawBezier(m3, 8, 8);
}

/* PURPOSE: renders the opengl objects onto the window.
 Clears window and chooses shader state first.
 calls a function to draw the objects and swaps window when done.
 */
void SdlApp::draw()
{
   handleAnimation();

   GLfloat matrix[16];

   //for each object
   for (int i = 0; i < sizeof(OBJECT_TYPES) / sizeof(OBJECT_TYPES[0]); i++)
   {
      //use specified shader
      const ShaderState& shaderstate = *shaderStates[i];
      glUseProgram(shaderStates[OBJECT_SHADE_MODE[i]]->program);

      //send projection matrix to shaders
      Matrix4::makeProjection(frustFovY,
         windowWidth / static_cast<double>(windowHeight), FRUST_NEAR,
         FRUST_FAR).writeToColumnMajorMatrix(matrix); //stick it in a matrix
      safe_glUniformMatrix4fv(shaderstate.h_uProjMatrix, matrix);

      //send texture
      safe_glUniform1i(shaderstate.h_uTexUnit, i);

      //send model view matrix
      Matrix4 MVM = inv(cameraposition) * objectpositions[i];
      MVM.writeToColumnMajorMatrix(matrix);
      safe_glUniformMatrix4fv(shaderstate.h_uModelViewMatrix, matrix);

      //send normal matrix
      normalMatrix(MVM).writeToColumnMajorMatrix(matrix);
      safe_glUniformMatrix4fv(shaderstate.h_uNormalMatrix, matrix);

      //draw
      objects[i]->draw(shaderstate);

      //gl function draws
      if (i == 0)
         drawMountains(shaderstate);
   }
}

/* PURPOSE: Executes the SDL application. Loops until event to quit.
 RETURN: -1 if fail, 0 success.
 */
int SdlApp::run()
{
   SDL_Event e;
   while (running)
   {
      while (SDL_PollEvent(&e))
         handleEvent(&e);

      clearCanvas();
      draw();
   }
   SDL_Quit();
   return 0;
}

/*  PURPOSE: main function. Makes an SDL application and executes it.
 RECEIVES:   number of args and arguments.
 */
int main(int argc, const char * argv[])
{
   //generate perlin noise points and put in arrays
   perlinNoise(m1, 3, 69105);
   perlinNoise(m2, 3, 0xdeadbeef);
   perlinNoise(m3, 3, 0xcafebabe);
   return SdlApp().run();
}

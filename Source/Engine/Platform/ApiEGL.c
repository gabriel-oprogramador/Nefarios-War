#ifdef PLATFORM_LINUX
#include "EGL/egl.h"
#include "GT/Engine.h"

typedef EGLNativeWindowType PWindow;
typedef EGLNativeDisplayType PDisplay;

extern Void ApiGLLoadFunctions(Void* LibGL);

static Void* SLibEGL = NULL;
static Void* SLibGL = NULL;

static String SLibEGLNames[] = {
    "eglGetDisplay",          //
    "eglInitialize",          //
    "eglBindAPI",             //
    "eglChooseConfig",        //
    "eglCreateWindowSurface", //
    "eglCreateContext",       //
    "eglMakeCurrent",         //
    "eglSwapBuffers",         //
    "\0"                      //
};

static struct {
  PFNEGLGETDISPLAYPROC eglGetDisplay;
  PFNEGLINITIALIZEPROC eglInitialize;
  PFNEGLBINDAPIPROC eglBindAPI;
  PFNEGLCHOOSECONFIGPROC eglChooseConfig;
  PFNEGLCREATEWINDOWSURFACEPROC eglCreateWindowSurface;
  PFNEGLCREATECONTEXTPROC eglCreateContext;
  PFNEGLMAKECURRENTPROC eglMakeCurrent;
  PFNEGLSWAPBUFFERSPROC eglSwapBuffers;
} SApiEGL;

static struct {
  EGLDisplay* display;
  EGLConfig config;
  EGLSurface* surface;
  EGLContext* context;
} SEglInfo;

Void ApiEGLSwapBuffer(){
  SApiEGL.eglSwapBuffers(SEglInfo.display, SEglInfo.surface);
}

Void ApiEGLInit(PWindow Window, PDisplay Display, Int32 Major, Int32 Minor, Int32 ColorBits, Int32 DepthBits) {
  SLibEGL = EngineLoadModule("libEGL.so");
  if (SLibEGL == NULL) {
    GT_LOG(LOG_FATAL, "API:EGL Not Loaded");
    return;
  }

  EngineLoadApi(SLibEGL, &SApiEGL, SLibEGLNames, false);
  GT_LOG(LOG_INFO, "API:EGL Initialized");

  PWindow win = Window;
  PDisplay dpy = SApiEGL.eglGetDisplay((PDisplay)Display);
  EGLConfig config;
  EGLSurface* surface;
  EGLContext* context;
  Int32 eglMajor;
  Int32 eglMinor;

  if (!SApiEGL.eglInitialize(dpy, &eglMajor, &eglMinor)) {
    GT_LOG(LOG_FATAL, "API:EGL UInitialized");
    return;
  }

  if (!SApiEGL.eglBindAPI(EGL_OPENGL_API)) {
    GT_LOG(LOG_FATAL, "API:EGL Not linked to OpenGL");
    return;
  }

  EGLint attributesList[] = {
      EGL_RENDERABLE_TYPE,         //
      EGL_OPENGL_BIT,              //
      EGL_RED_SIZE, ColorBits,     //
      EGL_GREEN_SIZE, ColorBits,   //
      EGL_BLUE_SIZE, ColorBits,    //
      EGL_DEPTH_SIZE, DepthBits,   //
      EGL_STENCIL_SIZE, ColorBits, //
      EGL_NONE,                    //
  };

  EGLint count;
  if (!SApiEGL.eglChooseConfig(dpy, attributesList, &config, true, &count) || count != 1) {
    GT_LOG(LOG_FATAL, "API:EGL There No Chosen Configuration.");
    return;
  }

  surface = SApiEGL.eglCreateWindowSurface(dpy, config, win, NULL);
  if (surface == EGL_NO_SURFACE) {
    GT_LOG(LOG_FATAL, "API:EGL Window Surface Not Created");
    return;
  }

  EGLint contextAttribList[] = {
      EGL_CONTEXT_MAJOR_VERSION, Major,                                     //
      EGL_CONTEXT_MINOR_VERSION, Minor,                                     //
      EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, //
#ifdef DEBUG_MODE
      EGL_CONTEXT_OPENGL_DEBUG, //
      EGL_TRUE,                 //
#endif                          //  DEBUG_MODE
      EGL_NONE,                 //
  };

  context = SApiEGL.eglCreateContext(dpy, config, EGL_NO_CONTEXT, contextAttribList);
  if (context == EGL_NO_CONTEXT) {
    GT_LOG(LOG_FATAL, "API:EGL Context Not Created");
    return;
  }

  if (!SApiEGL.eglMakeCurrent(dpy, surface, surface, context)) {
    GT_LOG(LOG_FATAL, "API:EGL It Didn't Make The Context Current.");
    return;
  }

  SEglInfo.display = dpy;
  SEglInfo.config = config;
  SEglInfo.surface = surface;
  SEglInfo.context = context;

  GT_LOG(LOG_INFO, "API:EGL Created OpenGL Context => Core Profile:%d.%d", Major, Minor);

  SLibGL = EngineLoadModule("libGL.so");
  ApiGLLoadFunctions(SLibGL);
}

#endif // PLATFORM_LINUX

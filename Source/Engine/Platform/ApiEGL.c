#include "CoreMinimal.h"
#ifdef PLATFORM_LINUX
#include "EGL/egl.h"
#include "GT/Engine.h"
#include "GT/Platform.h"

extern void ApiGLLoadFunctions(void* LibGL);

static void* SLibEGL = NULL;
static void* SLibGL = NULL;

static cstring SLibEGLNames[] = {
    "eglGetError",              //
    "eglGetConfigAttrib",       //
    "eglDestroySurface",        //
    "eglDestroyContext",        //
    "eglGetDisplay",            //
    "eglInitialize",            //
    "eglBindAPI",               //
    "eglChooseConfig",          //
    "eglCreatePbufferSurface",  //
    "eglCreateWindowSurface",   //
    "eglCreateContext",         //
    "eglMakeCurrent",           //
    "eglSwapBuffers",           //
    "\0"                        //
};

static struct {
  PFNEGLGETERRORPROC eglGetError;
  PFNEGLGETCONFIGSPROC eglGetConfigAttrib;
  PFNEGLDESTROYSURFACEPROC eglDestroySurface;
  PFNEGLDESTROYCONTEXTPROC eglDestroyContext;
  PFNEGLGETDISPLAYPROC eglGetDisplay;
  PFNEGLINITIALIZEPROC eglInitialize;
  PFNEGLBINDAPIPROC eglBindAPI;
  PFNEGLCHOOSECONFIGPROC eglChooseConfig;
  PFNEGLCREATEPBUFFERSURFACEPROC eglCreatePbufferSurface;
  PFNEGLCREATEWINDOWSURFACEPROC eglCreateWindowSurface;
  PFNEGLCREATECONTEXTPROC eglCreateContext;
  PFNEGLMAKECURRENTPROC eglMakeCurrent;
  PFNEGLSWAPBUFFERSPROC eglSwapBuffers;
} SApiEGL;

static struct {
  uint32 versionMajor;
  uint32 versionMinor;
  EGLDisplay* display;
  EGLConfig config;
  EGLSurface* surface;
  EGLContext* context;
  int32 attribList[16];
  int32 contextLis[16];
} SGLInfo;

bool ApiEGLInitOpenGL(int32 Major, int32 Minor) {
  SGLInfo.display = (EGLDisplay*)SApiEGL.eglGetDisplay(EGL_DEFAULT_DISPLAY);
  int32 colorBits = 8;
  int32 depthBits = 24;

  EGLint attributesList[] = {
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,  //
      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,                 //
      EGL_RED_SIZE, colorBits,                             //
      EGL_GREEN_SIZE, colorBits,                           //
      EGL_BLUE_SIZE, colorBits,                            //
      EGL_ALPHA_SIZE, colorBits,                           //
      EGL_DEPTH_SIZE, depthBits,                           //
      EGL_NONE,                                            //
  };

  EGLint pBufferList[] = {
      EGL_WIDTH, 1,                               //
      EGL_HEIGHT, 1,                              //
      EGL_NONE,                                   //
  };

  EGLint contextAttribList[] = {
      EGL_CONTEXT_MAJOR_VERSION, Major,                                      //
      EGL_CONTEXT_MINOR_VERSION, Minor,                                      //
      EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,  //
#ifdef DEBUG_MODE
      EGL_CONTEXT_OPENGL_DEBUG,  //
      EGL_TRUE,                  //
#endif                           //  DEBUG_MODE
      EGL_NONE,                  //
  };

  if(!SApiEGL.eglInitialize(SGLInfo.display, NULL, NULL)) {
    GT_LOG(LOG_FATAL, "API:EGL UInitialized");
    return false;
  }

  SApiEGL.eglBindAPI(EGL_OPENGL_API);

  EGLint numConfigs;
  if(!SApiEGL.eglChooseConfig(SGLInfo.display, attributesList, &SGLInfo.config, 1, &numConfigs) || numConfigs < 1) {
    GT_LOG(LOG_FATAL, "API:EGL There No Chosen Configuration.");
    return false;
  }

  SGLInfo.surface = SApiEGL.eglCreatePbufferSurface(SGLInfo.display, SGLInfo.config, pBufferList);
  if(SGLInfo.surface == EGL_NO_SURFACE) {
    GT_LOG(LOG_FATAL, "API:EGL Buffer Surface Not Created");
    return false;
  }

  SGLInfo.context = SApiEGL.eglCreateContext(SGLInfo.display, SGLInfo.config, NULL, contextAttribList);
  if(SGLInfo.context == EGL_NO_CONTEXT) {
    GT_LOG(LOG_FATAL, "API:EGL Context Not Created");
    return false;
  }

  SApiEGL.eglMakeCurrent(SGLInfo.display, SGLInfo.surface, SGLInfo.surface, SGLInfo.context);

  ApiGLLoadFunctions(SLibGL);

  PMemCopy(&SGLInfo.attribList, &attributesList, sizeof(attributesList));
  PMemCopy(&SGLInfo.contextLis, &contextAttribList, sizeof(contextAttribList));

  SGLInfo.versionMajor = Major;
  SGLInfo.versionMinor = Minor;

  GT_LOG(LOG_INFO, "API-EGL: Created OpenGL Context => Core Profile:%d.%d", Major, Minor);
  return true;
}

bool ApiEGLCreateContext() {
  EGLNativeWindowType win = (EGLNativeWindowType)GEngine.mainWindow.pWindow;

  SApiEGL.eglDestroySurface(SGLInfo.display, SGLInfo.surface);

  SGLInfo.surface = SApiEGL.eglCreateWindowSurface(SGLInfo.display, SGLInfo.config, win, NULL);
  if(SGLInfo.surface == EGL_NO_SURFACE) {
    int error = SApiEGL.eglGetError();
    GT_LOG(LOG_FATAL, "API:EGL Window Surface Not Created -> %d", error);
    return false;
  }

  if(!SApiEGL.eglMakeCurrent(SGLInfo.display, SGLInfo.surface, SGLInfo.surface, SGLInfo.context)) {
    GT_LOG(LOG_FATAL, "API:EGL It Didn't Make The Context Current.");
    return false;
  }

  GEngine.mainWindow.pContext = (void*)SGLInfo.context;
  return true;
}

void ApiEGLDestroyContext() {
  SApiEGL.eglMakeCurrent(0, 0, 0, 0);
  SApiEGL.eglDestroyContext(SGLInfo.display, SGLInfo.context);
  GEngine.mainWindow.pContext = NULL;
}

void ApiEGLMakeCurrent() {
  SApiEGL.eglMakeCurrent(SGLInfo.display, SGLInfo.surface, SGLInfo.surface, SGLInfo.context);
}

void ApiEGLSwapBuffers() {
  SApiEGL.eglSwapBuffers(SGLInfo.display, SGLInfo.surface);
}

bool ApiEGLInit(IGraphicApi* Api) {
  SLibEGL = PModuleLoad("libEGL.so");
  SLibGL = PModuleLoad("libGL.so");

  if(SLibEGL == NULL) {
    GT_LOG(LOG_FATAL, "Not Loaded EGL");
    return false;
  }
  if(SLibGL == NULL) {
    GT_LOG(LOG_FATAL, "Not Loaded OpenGL");
    return false;
  }

  PModuleLoadApi(SLibEGL, &SApiEGL, SLibEGLNames, false);

  Api->OnInitOpenGL = &ApiEGLInitOpenGL;
  Api->OnCreateContext = &ApiEGLCreateContext;
  Api->OnDestroyContext = &ApiEGLDestroyContext;
  Api->OnMakeCurrent = &ApiEGLMakeCurrent;
  Api->OnSwapBuffers = &ApiEGLSwapBuffers;

  return true;
}

#endif  // PLATFORM_LINUX

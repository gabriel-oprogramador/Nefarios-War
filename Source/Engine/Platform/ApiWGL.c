#include "CoreMinimal.h"

#ifdef PLATFORM_WINDOWS
#include "GT/Engine.h"
#include "GT/Platform.h"
#include "GL/glcorearb.h"
#include "GL/wglext.h"
#include <windows.h>

extern void ApiGLLoadFunctions(void* LibGL);

static void* SLibGdi = NULL;
static void* SLibGL = NULL;

static cstring SLibGdiNames[] = {
    "ChoosePixelFormat",
    "SetPixelFormat",
    "SwapBuffers",
    "\0"};

// clang-format off
static struct {
  int (*ChoosePixelFormat)(HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd);
  BOOL (*SetPixelFormat)(HDC hdc, int format, const PIXELFORMATDESCRIPTOR* ppfd);
  BOOL (*SwapBuffers)(HDC unnamedParam1);
} SApiGdi;
// clang-format on

static cstring SLibWglNames[] = {
    "wglCreateContext",
    "wglMakeCurrent",
    "wglGetProcAddress",
    "wglDeleteContext",
    "wglSwapLayerBuffers",
    "\0"};

// clang-format off
static struct {
  HGLRC(*wglCreateContext) (HDC unnamedParam1);
  BOOL(*wglMakeCurrent) (HDC unnamedParam1, HGLRC unnamedParam2);
  PROC(*wglGetProcAddress) (LPCSTR unnamedParam1);
  BOOL(*wglDeleteContext) (HGLRC unnamedParam1);
  BOOL(*wglSwapLayerBuffers) (HDC, UINT);
} SApiWgl;
// clang-format on

static struct {
  HGLRC context;
  HDC device;
  uint32 versionMajor;
  uint32 versionMinor;
  PFNWGLCHOOSEPIXELFORMATARBPROC OnWglChoosePixelFormatARB;
  PFNWGLCREATECONTEXTATTRIBSARBPROC OnWglCreateContextAttribsARB;
  int32 pixelFormatAttribs[16];
  int32 contextAttribs[16];
} SOpenGLInfo;

bool ApiWglInitOpenGL(int32 Major, int32 Minor) {
  GEngine.windowApi.OnCreate(0, 0, "", 0);
  HDC dc = (HDC)GEngine.mainWindow.pDevice;
  BYTE colorBits = 32;
  BYTE depthBits = 24;

  // clang-format off
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    depthBits,
    0,0,0,0,0,0,
    0,
    0,
    0,
    0,0,0,0,
    colorBits,
    0,
    0,
    PFD_MAIN_PLANE,
    0,
    0,0,0,
  };
  // clang-format on

  int32 pixeFormat = SApiGdi.ChoosePixelFormat(dc, &pfd);
  if(!pixeFormat) {
    GT_LOG(LOG_FATAL, "Not Choose Pixel Format");
    GEngine.windowApi.OnDestroy();
    return false;
  }

  if(!SApiGdi.SetPixelFormat(dc, pixeFormat, &pfd)) {
    GT_LOG(LOG_FATAL, "Not Set Pixel Format");
    GEngine.windowApi.OnDestroy();
    return false;
  }

  HGLRC rc = SApiWgl.wglCreateContext(dc);
  if(rc == NULL) {
    GT_LOG(LOG_FATAL, "Not Create Context");
    GEngine.windowApi.OnDestroy();
    return false;
  }

  if(!SApiWgl.wglMakeCurrent(dc, rc)) {
    GT_LOG(LOG_FATAL, "Not make current context");
    GEngine.windowApi.OnDestroy();
    return false;
  }

  // clang-format off
  int32 pixelFormatAttribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_COLOR_BITS_ARB, colorBits,
    WGL_DEPTH_BITS_ARB, depthBits,
    WGL_SAMPLE_BUFFERS_ARB, 1,
    WGL_SAMPLES_ARB, 4,
    0
  };

  int32 contextAttribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, Major,
    WGL_CONTEXT_MINOR_VERSION_ARB, Minor,
    WGL_CONTEXT_FLAGS_ARB,
#ifdef DEBUG_MODE
    WGL_CONTEXT_DEBUG_BIT_ARB |
#endif // DEBUG_MODE
    WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };
  // clang-format on

  SOpenGLInfo.versionMajor = Major;
  SOpenGLInfo.versionMinor = Minor;
  SOpenGLInfo.context = NULL;
  SOpenGLInfo.OnWglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)SApiWgl.wglGetProcAddress("wglChoosePixelFormatARB");
  SOpenGLInfo.OnWglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)SApiWgl.wglGetProcAddress("wglCreateContextAttribsARB");
  PMemCopy(&SOpenGLInfo.pixelFormatAttribs, &pixelFormatAttribs, sizeof(pixelFormatAttribs));
  PMemCopy(&SOpenGLInfo.contextAttribs, &contextAttribs, sizeof(contextAttribs));

  // Destroy Old Context
  SApiWgl.wglMakeCurrent(0, 0);
  SApiWgl.wglDeleteContext(rc);
  GEngine.windowApi.OnDestroy();

  // Make Modern Context
  GEngine.windowApi.OnCreate(100, 100, "OpenGL", 0);
  GEngine.graphicApi.OnCreateContext();
  GEngine.graphicApi.OnMakeCurrent();

  ApiGLLoadFunctions(SLibGL);
  GEngine.windowApi.OnDestroy();
  GT_LOG(LOG_INFO, "API-WGL: Created OpenGL Context => %d.%d", SOpenGLInfo.versionMajor, SOpenGLInfo.versionMinor);

  return true;
}

bool ApiWglCreateContext() {
  HDC dc = (HDC)GEngine.mainWindow.pDevice;
  int32 pixelFormat = 0;
  uint32 numPixelFormat = 0;
  PIXELFORMATDESCRIPTOR pfd;

  const int32* pixelFormatList = (const int32*)SOpenGLInfo.pixelFormatAttribs;
  const int32* contextList = (const int32*)SOpenGLInfo.contextAttribs;

  SOpenGLInfo.OnWglChoosePixelFormatARB(dc, pixelFormatList, NULL, 1, &pixelFormat, (UINT*)&numPixelFormat);
  if(numPixelFormat <= 0) {
    GT_LOG(LOG_FATAL, "Not valid Pixel Format");
    return false;
  }

  if(!SApiGdi.SetPixelFormat(dc, pixelFormat, &pfd)) {
    GT_LOG(LOG_FATAL, "Not set modern Pixel Format");
    return false;
  }

  HGLRC rc = SOpenGLInfo.context;
  if(rc == NULL) {
    rc = SOpenGLInfo.OnWglCreateContextAttribsARB(dc, NULL, contextList);
    SOpenGLInfo.context = rc;
  }

  if(rc == NULL) {
    GT_LOG(LOG_FATAL, "Not create modern context");
    return false;
  }

  GEngine.mainWindow.pContext = (void*)rc;

  return true;
}

void ApiWglDestroyContext() {
  if(GEngine.mainWindow.pContext == NULL) {
    return;
  }
  SApiWgl.wglMakeCurrent(0, 0);
  SApiWgl.wglDeleteContext((HGLRC)GEngine.mainWindow.pContext);
}

void ApiWglMakeCurrent() {
  if(GEngine.mainWindow.pContext == NULL) {
    return;
  }
  SApiWgl.wglMakeCurrent((HDC)GEngine.mainWindow.pDevice, (HGLRC)GEngine.mainWindow.pContext);
}

void ApiWglSwapBuffers() {
  SApiGdi.SwapBuffers((HDC)GEngine.mainWindow.pDevice);
}

bool ApiWglInit(IGraphicApi* Api) {
  SLibGdi = PModuleLoad("gdi32.dll");
  SLibGL = PModuleLoad("opengl32.dll");

  if(SLibGdi == NULL || SLibGL == NULL) {
    GT_LOG(LOG_FATAL, "Not loaded Gdi32.dll or OpenGL32.dll");
    return false;
  }

  PModuleLoadApi(SLibGdi, &SApiGdi, SLibGdiNames, false);
  PModuleLoadApi(SLibGL, &SApiWgl, SLibWglNames, false);

  Api->OnInitOpenGL = &ApiWglInitOpenGL;
  Api->OnCreateContext = &ApiWglCreateContext;
  Api->OnDestroyContext = &ApiWglDestroyContext;
  Api->OnMakeCurrent = &ApiWglMakeCurrent;
  Api->OnSwapBuffers = &ApiWglSwapBuffers;
  GT_LOG(LOG_INFO, "API-WGL: Initialized");

  return true;
}

#endif  // PLATFORM_WINDOWS

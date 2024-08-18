#ifdef PLATFORM_WINDOWS
#include "Engine.h"
#include "GL/glcorearb.h"
#include "GL/wglext.h"

#include <windows.h>

extern HWND ApiWin32CreateWindow(Int32 Width, Int32 Height, String Title);
extern Void ApiWin32DestroyWindow(HWND Window);
extern HDC ApiWin32GetDC(HWND Window);
extern void ApiGLLoadFuntions(void* LibOpenGL, Bool bDebugMode);

static Void* SLibGdi32 = NULL;
static Void* SLibGL32 = NULL;

static String SLibGdi32Names[] = {
    "ChoosePixelFormat", //
    "SetPixelFormat",    //
    "SwapBuffers",       //
    "\0"                 //
};

static struct {
  int (*ChoosePixelFormat)(HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd);
  BOOL(*SetPixelFormat) (HDC hdc, int format, const PIXELFORMATDESCRIPTOR* ppfd);
  BOOL(*SwapBuffers) (HDC unnamedParam1);
} SApiGdi;

static String SLibWglNames[] = {
    "wglCreateContext",    //
    "wglMakeCurrent",      //
    "wglGetProcAddress",   //
    "wglDeleteContext",    //
    "wglSwapLayerBuffers", //
    "\0"                   //
};

static struct {
  HGLRC(*wglCreateContext)
  (HDC unnamedParam1);
  BOOL(*wglMakeCurrent)
  (HDC unnamedParam1, HGLRC unnamedParam2);
  PROC(*wglGetProcAddress)
  (LPCSTR unnamedParam1);
  BOOL(*wglDeleteContext)
  (HGLRC unnamedParam1);
  BOOL(*wglSwapLayerBuffers)
  (HDC, UINT);
} SApiWgl;

Void ApiGdiSwapBuffer(HDC Device){
  SApiGdi.SwapBuffers(Device);
}

HGLRC ApiWglInit(HWND Window, HDC Device, Int32 Major, Int32 Minor, Int32 ColorBits, Int32 DepthBits) {
  HWND dummyWindow = ApiWin32CreateWindow(0, 0, "Dummy Window");
  HDC dummyDc = ApiWin32GetDC(dummyWindow);
  HGLRC renderContext = NULL;

  SLibGdi32 = EngineLoadModule("gdi32.dll");
  SLibGL32 = EngineLoadModule("opengl32.dll");

  if (SLibGdi32 == NULL || SLibGL32 == NULL) {
    GT_LOG(LOG_FATAL, "API:WGL Lib gdi32 or opengl32 not loaded");
    ApiWin32DestroyWindow(dummyWindow);
    return NULL;
  }

  EngineLoadApi(SLibGdi32, &SApiGdi, SLibGdi32Names, false);
  EngineLoadApi(SLibGL32, &SApiWgl, SLibWglNames, false);

  GT_LOG(LOG_INFO, "API:WGL Initialized");

  PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      DepthBits,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      ColorBits,
      0, 0,
      PFD_MAIN_PLANE,
      0, 0, 0, 0};

  Int32 pixelFormat = SApiGdi.ChoosePixelFormat(dummyDc, &pfd);
  if (!pixelFormat) {
    GT_LOG(LOG_FATAL, "API:WGL Not choose Pixel Format");
    ApiWin32DestroyWindow(dummyWindow);
    return NULL;
  }

  if (!SApiGdi.SetPixelFormat(dummyDc, pixelFormat, &pfd)) {
    GT_LOG(LOG_FATAL, "API:WGL Not set Pixel Format");
    ApiWin32DestroyWindow(dummyWindow);
    return NULL;
  }

  renderContext = SApiWgl.wglCreateContext(dummyDc);
  if (!renderContext) {
    GT_LOG(LOG_FATAL, "API:WGL Render Context Invalid");
    ApiWin32DestroyWindow(dummyWindow);
    return NULL;
  }

  if (!SApiWgl.wglMakeCurrent(dummyDc, renderContext)) {
    GT_LOG(LOG_FATAL, "API:WGL Not make current Reder Context");
    ApiWin32DestroyWindow(dummyWindow);
    return NULL;
  }

  PFNWGLCHOOSEPIXELFORMATARBPROC OnWglChoosePixelFormatARB =
      (PFNWGLCHOOSEPIXELFORMATARBPROC)SApiWgl.wglGetProcAddress("wglChoosePixelFormatARB");
  PFNWGLCREATECONTEXTATTRIBSARBPROC OnWglCreateContextAttribsARB =
      (PFNWGLCREATECONTEXTATTRIBSARBPROC)SApiWgl.wglGetProcAddress("wglCreateContextAttribsARB");

  SApiWgl.wglMakeCurrent(0, 0);
  SApiWgl.wglDeleteContext(renderContext);
  ApiWin32DestroyWindow(dummyWindow);

  Int32 pixelFormatCount;
  Int32 pixelFormatAttribList[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_COLOR_BITS_ARB, ColorBits,
      WGL_DEPTH_BITS_ARB, DepthBits};

  OnWglChoosePixelFormatARB(Device, pixelFormatAttribList, NULL, 1, &pixelFormat, (UINT*)&pixelFormatCount);
  if (pixelFormatCount < 0) {
    GT_LOG(LOG_FATAL, "API:WGL Not choose modern Pixel Format");
    return NULL;
  }

  if (!SApiGdi.SetPixelFormat(Device, pixelFormat, &pfd)) {
    GT_LOG(LOG_FATAL, "API:WGL Not set modern Pixel Format");
    return NULL;
  }

  Int32 contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      Major,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      Minor,
      WGL_CONTEXT_FLAGS_ARB,
#ifdef DEBUG_MODE
      WGL_CONTEXT_DEBUG_BIT_ARB |
#endif // DEBUG_MODE
          WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0};

  renderContext = OnWglCreateContextAttribsARB(Device, 0, contextAttribs);
  if (!renderContext) {
    GT_LOG(LOG_FATAL, "API:WGL Not Create Modern Context");
    return NULL;
  }

  if (!SApiWgl.wglMakeCurrent(Device, renderContext)) {
    GT_LOG(LOG_FATAL, "API:WGL Not Make Current Modern Context");
    return NULL;
  }

  GT_LOG(LOG_INFO, "API:WGL Created OpenGL Context => Core Profile:%d.%d", Major, Minor);
  ApiGLLoadFuntions(SLibGL32, false);

  return renderContext;
}

#endif // PLATFORM_WINDOWS

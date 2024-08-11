#ifdef PLATFORM_LINUX
#include "Engine.h"

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// To avoid external resets
#ifdef Bool
#undef Bool
#endif // Bool

static Void* SLibX11 = NULL;
static String SApiX11Name[] = {
    "XCreateBitmapFromData",      //
    "XCreatePixmapCursor",        //
    "XFreePixmap",                //
    "XCreateFontCursor",          //
    "XDefineCursor",              //
    "XFreeCursor",                //
    "XkbSetDetectableAutoRepeat", //
    "XInternAtom",                //
    "XSetWMProtocols",            //
    "XStoreName",                 //
    "XOpenDisplay",               //
    "XCloseDisplay",              //
    "XMapWindow",                 //
    "XUnmapWindow",               //
    "XSelectInput",               //
    "XNextEvent",                 //
    "XSendEvent",                 //
    "XPending",                   //
    "XSetWMNormalHints",          //
    "XAllocSizeHints",            //
    "XLookupKeysym",              //
    "XDestroyWindow",             //
    "XCreateSimpleWindow",        //
    "\0"                          //
};

static struct {
  Pixmap (*XCreateBitmapFromData)(Display*, Drawable, const char*, unsigned int, unsigned int);
  Cursor (*XCreatePixmapCursor)(Display*, Pixmap, Pixmap, XColor*, XColor*, unsigned int, unsigned int);
  int (*XFreePixmap)(Display*, Pixmap);
  Cursor (*XCreateFontCursor)(Display*, unsigned int);
  int (*XDefineCursor)(Display*, Window, Cursor);
  int (*XFreeCursor)(Display*, Cursor);
  int (*XkbSetDetectableAutoRepeat)(Display*, int, int*);
  Atom (*XInternAtom)(Display*, const char*, int);
  Atom (*XSetWMProtocols)(Display*, Window, Atom*, int);
  int (*XStoreName)(Display*, Window, const char*);
  Display* (*XOpenDisplay)(const char*);
  int (*XCloseDisplay)(Display*);
  int (*XMapWindow)(Display*, Window);
  int (*XUnmapWindow)(Display*, Window);
  int (*XSelectInput)(Display*, Window, long);
  int (*XNextEvent)(Display*, XEvent*);
  int (*XSendEvent)(Display*, Window, int, long, XEvent*);
  int (*XPending)(Display*);
  void (*XSetWMNormalHints)(Display*, Window, XSizeHints*);
  XSizeHints* (*XAllocSizeHints)();
  KeySym (*XLookupKeysym)(XKeyEvent*, int);
  int (*XDestroyWindow)(Display*, Window);
  Window (*XCreateSimpleWindow)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
} SApiX11;

static struct {
  Int32 width;
  Int32 height;
  String title;
  Bool bFullsreen;
  Bool bShouldClose;
  Window window;
  Display* display;
  XSizeHints* sizeHint;
  Atom wmProtocols;
  Atom wmDeleteWindow;
} SWindow;

Void ApiX11WindowCreate(Int32 Width, Int32 Height, String Title) {
  Int32 keyMask = KeyPressMask | KeyReleaseMask;
  Int32 buttonMask = ButtonPressMask | ButtonReleaseMask;
  Int32 otherMask = StructureNotifyMask | PointerMotionMask;
  Int32 mask = keyMask | buttonMask | otherMask;

  Display* dpy = SApiX11.XOpenDisplay(NULL);
  Int32 screen = DefaultScreen(dpy);
  UInt32 blackPixel = BlackPixel(dpy, screen);
  UInt32 whitePixe = WhitePixel(dpy, screen);
  Window win = SApiX11.XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, Width, Height, 0, blackPixel, whitePixe);
  SApiX11.XMapWindow(dpy, win);
  SApiX11.XSelectInput(dpy, win, mask);
  SApiX11.XStoreName(dpy, win, Title);
  SApiX11.XkbSetDetectableAutoRepeat(dpy, true, NULL);
  SWindow.wmProtocols = SApiX11.XInternAtom(dpy, "WM_PROTOCOLS", false);
  SWindow.wmDeleteWindow = SApiX11.XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  SApiX11.XSetWMProtocols(dpy, win, &SWindow.wmDeleteWindow, true);

  SWindow.sizeHint = SApiX11.XAllocSizeHints();
  SWindow.sizeHint->flags = PMinSize | PMaxSize;
  SWindow.sizeHint->min_width = Width;
  SWindow.sizeHint->min_height = Height;
  SWindow.sizeHint->max_width = Width;
  SWindow.sizeHint->max_height = Height;
  SApiX11.XSetWMNormalHints(dpy, win, SWindow.sizeHint);

  SWindow.width = Width;
  SWindow.height = Height;
  SWindow.title = Title;
  SWindow.bFullsreen = false;
  SWindow.bShouldClose = false;
  SWindow.display = dpy;
  SWindow.window = win;
  GT_LOG(LOG_INFO, "API:X11 Create Window => Width:%d Height:%d Title:%s", Width, Height, Title);
}

Void ApiX11WindowUpdate() {
  XEvent event;
  while (SApiX11.XPending(SWindow.display)) {
    SApiX11.XNextEvent(SWindow.display, &event);
    switch (event.type) {
      case KeyPress: {
        SWindow.bShouldClose = true;
      } break;
      case KeyRelease: {
      } break;
      case ButtonPress: {
      } break;
      case ButtonRelease: {
      } break;
      case ClientMessage: {
        if (event.xclient.message_type == SWindow.wmProtocols) {
          if (event.xclient.data.l[0] == (Int32)SWindow.wmDeleteWindow) {
            SWindow.bShouldClose = true;
          }
        }
      } break;
    }
  }
}

Void ApiX11WindowDestroy() {
  SApiX11.XUnmapWindow(SWindow.display, SWindow.window);
  SApiX11.XDestroyWindow(SWindow.display, SWindow.window);
  SApiX11.XCloseDisplay(SWindow.display);
  GT_LOG(LOG_INFO, "API:X11 Close Window");
}

Bool ApiX11WindowShouldClose() {
  return SWindow.bShouldClose;
}

Bool ApiX11Init(FGT* Engine) {
  SLibX11 = EngineLoadModule("libX11.so");
  if (SLibX11 == NULL) {
    return false;
  }
  EngineLoadApi(SLibX11, &SApiX11, SApiX11Name, false);
  Engine->WindowCreate = &ApiX11WindowCreate;
  Engine->WindowUpdate = &ApiX11WindowUpdate;
  Engine->WindowDestroy = &ApiX11WindowDestroy;
  Engine->WindowShouldClose = &ApiX11WindowShouldClose;
  return true;
}

#endif // PLATFORM_LINUX

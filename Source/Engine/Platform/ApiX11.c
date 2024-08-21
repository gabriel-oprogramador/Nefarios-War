#ifdef PLATFORM_LINUX
#include "EGL/eglplatform.h"
#include "Engine.h"

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// To avoid external resets
#ifdef Bool
#undef Bool
#endif // Bool

typedef EGLNativeWindowType PWindow;
typedef EGLNativeDisplayType PDisplay;

extern Void ApiEGLInit(PWindow Window, PDisplay Display, Int32 Major, Int32 Minor, Int32 ColorBits, Int32 DepthBits);

static Void* SLibX11 = NULL;
static String SApiX11Name[] = {
    "XWarpPointer",               //
    "XUndefineCursor",            //
    "XDisplayKeycodes",           //
    "XGetKeyboardMapping",        //
    "XFree",                      //
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
  int (*XWarpPointer)(Display*, Window, Window, int, int, unsigned int, unsigned int, int, int);
  int (*XUndefineCursor)(Display*, Window);
  int (*XDisplayKeycodes)(Display*, int*, int*);
  KeySym* (*XGetKeyboardMapping)(Display*, KeyCode, int, int*);
  int (*XFree)(void*);
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
  Window window;
  Display* display;
  XSizeHints* sizeHint;
  Atom wmProtocols;
  Atom wmDeleteWindow;
} SWindow;

static struct {
  KeySym* map;
  Int32 keyCodeMin;
  Int32 keyCodeMax;
  Int32 keyCodeCount;
  Int32 keySymPerkeyCode;
} SKeysMapping;

static Void InternalUpdateWindowTitle();
static Void InternalGetKeybordMapping();
static Void InternalUpdateKey(KeyCode Code, Bool bIsButton, Bool bIsPressed);

Void ApiX11WindowCreate(Int32 Width, Int32 Height, String Title) {
  Int32 keyMask = KeyPressMask | KeyReleaseMask;
  Int32 buttonMask = ButtonPressMask | ButtonReleaseMask;
  Int32 otherMask = StructureNotifyMask | PointerMotionMask;
  Int32 mask = keyMask | buttonMask | otherMask;

  Display* dpy = SApiX11.XOpenDisplay(NULL);
  Int32 screen = DefaultScreen(dpy);
  UInt32 blackPixel = BlackPixel(dpy, screen);
  UInt32 whitePixe = WhitePixel(dpy, screen);
  Window win = SApiX11.XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, Width, Height, 1, blackPixel, whitePixe);
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

  GEngine.windowApi.width = Width;
  GEngine.windowApi.height = Height;
  GEngine.windowApi.title = Title;
  GEngine.windowApi.bFullscreen = false;
  GEngine.windowApi.bShouldClose = false;
  SWindow.display = dpy;
  SWindow.window = win;
  InternalGetKeybordMapping();
  GT_LOG(LOG_INFO, "API:X11 Created Window => Width:%d Height:%d Title:%s", Width, Height, Title);
  ApiEGLInit(SWindow.window, SWindow.display, 3, 3, 8, 24); // ColorBits R:8 G:8 B:8 A:8
}

Void ApiX11WindowUpdate() {
#ifdef DEBUG_MODE
  InternalUpdateWindowTitle();
#endif // DEBUG_MODE

  XEvent event;
  memcpy(GEngine.inputApi.previousKeys, GEngine.inputApi.currentKeys, sizeof(GEngine.inputApi.previousKeys));

  while (SApiX11.XPending(SWindow.display)) {
    SApiX11.XNextEvent(SWindow.display, &event);
    switch (event.type) {
      case KeyPress: {
        InternalUpdateKey(event.xkey.keycode, false, true);
      } break;
      case KeyRelease: {
        InternalUpdateKey(event.xkey.keycode, false, false);
      } break;
      case ButtonPress: {
        InternalUpdateKey(event.xbutton.button, true, true);
      } break;
      case ButtonRelease: {
        InternalUpdateKey(event.xbutton.button, true, false);
      } break;
      case ConfigureNotify: {
        GEngine.windowApi.width = event.xconfigurerequest.width;
        GEngine.windowApi.height = event.xconfigurerequest.height;
        // TODO:Update The Renderer Viewport.
      } break;
      case MotionNotify: {
        if (event.xmotion.window == SWindow.window) {
          Float posX = event.xmotion.x;
          Float posY = event.xmotion.y;
          posX = (posX > GEngine.windowApi.width) ? GEngine.windowApi.width : posX;
          posY = (posY > GEngine.windowApi.width) ? GEngine.windowApi.height : posY;
          GEngine.inputApi.mousePosition[0] = posX;
          GEngine.inputApi.mousePosition[1] = posY;
        }
      } break;
      case ClientMessage: {
        if (event.xclient.message_type == SWindow.wmProtocols) {
          if (event.xclient.data.l[0] == (Int32)SWindow.wmDeleteWindow) {
            GEngine.windowApi.bShouldClose = true;
          }
        }
      } break;
    }
  }
}

Void ApiX11WindowDestroy() {
  GEngine.windowApi.bShouldClose = true;
  SApiX11.XUnmapWindow(SWindow.display, SWindow.window);
  SApiX11.XDestroyWindow(SWindow.display, SWindow.window);
  SApiX11.XCloseDisplay(SWindow.display);
  GT_LOG(LOG_INFO, "API:X11 Closed Window");
}

Void ApiX11WindowFullscreen(Bool bIsFullscreen) {
  XEvent event;
  GEngine.windowApi.bFullscreen = !GEngine.windowApi.bFullscreen;
  SWindow.sizeHint->flags = (bIsFullscreen) ? 0 : PMinSize | PMaxSize;
  SApiX11.XSetWMNormalHints(SWindow.display, SWindow.window, SWindow.sizeHint);
  Atom wmState = SApiX11.XInternAtom(SWindow.display, "_NET_WM_STATE", false);
  Atom wmFullscreen = SApiX11.XInternAtom(SWindow.display, "_NET_WM_STATE_FULLSCREEN", false);
  memset(&event, 0, 1);
  event.type = ClientMessage;
  event.xclient.window = SWindow.window;
  event.xclient.message_type = wmState;
  event.xclient.format = 32;
  event.xclient.data.l[0] = bIsFullscreen;
  event.xclient.data.l[1] = wmFullscreen;
  event.xclient.data.l[2] = 0;
  SApiX11.XSendEvent(SWindow.display, DefaultRootWindow(SWindow.display), false, StructureNotifyMask, &event);
}

Void ApiX11WindowShowCursor(Bool bShow) {
  GEngine.windowApi.bShowCursor = bShow;

  if (bShow) {
    SApiX11.XUndefineCursor(SWindow.display, SWindow.window);
  } else {
    Pixmap noData;
    Cursor invisibleCursor;
    XColor black;
    static Char noDataBits[] = {0};
    noData = SApiX11.XCreateBitmapFromData(SWindow.display, SWindow.window, noDataBits, 1, 1);
    black.red = black.green = black.blue = 0;
    invisibleCursor = SApiX11.XCreatePixmapCursor(SWindow.display, noData, noData, &black, &black, 0, 0);
    SApiX11.XDefineCursor(SWindow.display, SWindow.window, invisibleCursor);
  }
}

Void ApiX11SetCursorPos(UInt32 X, UInt32 Y) {
  SApiX11.XWarpPointer(SWindow.display, None, SWindow.window, 0, 0, 0, 0, X, Y);
}

static Void InternalUpdateWindowTitle() {
  Char buffer[BUFFER_SMALL] = "";
  String title = GEngine.windowApi.title;
  Double deltaTime = GEngine.timerApi.deltaTime;
  UInt32 frameRate = GEngine.timerApi.frameRate;
  snprintf(buffer, BUFFER_SMALL, "%s (Debug Mode) => FPS:%u | MS:%f", title, frameRate, deltaTime);
  SApiX11.XStoreName(SWindow.display, SWindow.window, buffer);
}

static Void InternalGetKeybordMapping() {
  SApiX11.XDisplayKeycodes(SWindow.display, &SKeysMapping.keyCodeMin, &SKeysMapping.keyCodeMax);
  SKeysMapping.keyCodeCount = SKeysMapping.keyCodeMax - SKeysMapping.keyCodeMin + 1;
  SKeysMapping.map = SApiX11.XGetKeyboardMapping(SWindow.display, SKeysMapping.keyCodeMin, SKeysMapping.keyCodeCount, &SKeysMapping.keySymPerkeyCode);
}

static Void InternalUpdateKey(KeyCode Code, Bool bIsButton, Bool bIsPressed) {
  KeySym keySym = 0;
  if (bIsButton) {
    keySym = Code;
  } else {
    Int32 index = (Code - SKeysMapping.keyCodeMin) * SKeysMapping.keySymPerkeyCode;
    keySym = SKeysMapping.map[index];
    keySym = (keySym >= XK_a && keySym <= XK_z) ? keySym - (XK_a - XK_A) : keySym;
  }

  static UInt16 keySymCode[] = {
      XK_apostrophe,      // Key: '
      XK_comma,           // Key: ,
      XK_minus,           // Key: -
      XK_period,          // Key: .
      XK_slash,           // Key: /
      XK_0,               // Key: 0
      XK_1,               // Key: 1
      XK_2,               // Key: 2
      XK_3,               // Key: 3
      XK_4,               // Key: 4
      XK_5,               // Key: 5
      XK_6,               // Key: 6
      XK_7,               // Key: 7
      XK_8,               // Key: 8
      XK_9,               // Key: 9
      XK_semicolon,       // Key: ;
      XK_equal,           // Key: =
      XK_A,               // Key: A
      XK_B,               // Key: B
      XK_C,               // Key: C
      XK_D,               // Key: D
      XK_E,               // Key: E
      XK_F,               // Key: F
      XK_G,               // Key: G
      XK_H,               // Key: H
      XK_I,               // Key: I
      XK_J,               // Key: J
      XK_K,               // Key: K
      XK_L,               // Key: L
      XK_M,               // Key: M
      XK_N,               // Key: N
      XK_O,               // Key: O
      XK_P,               // Key: P
      XK_Q,               // Key: Q
      XK_R,               // Key: R
      XK_S,               // Key: S
      XK_T,               // Key: T
      XK_U,               // Key: U
      XK_V,               // Key: V
      XK_W,               // Key: W
      XK_X,               // Key: X
      XK_Y,               // Key: Y
      XK_Z,               // Key: Z
      XK_bracketleft,     // Key: [{
      XK_backslash,       // Key: '\'
      XK_bracketright,    // Key: ]}
      XK_grave,           // Key: `
      XK_space,           // Key: Space
      XK_Escape,          // Key: Escape
      XK_Return,          // Key: Enter
      XK_Tab,             // Key: Tab
      XK_BackSpace,       // Key: Backspace
      XK_Insert,          // Key: Insert
      XK_Delete,          // Key: Delete
      XK_Right,           // Key: Right
      XK_Left,            // Key: Left
      XK_Down,            // Key: Down
      XK_Up,              // Key: Up
      XK_Page_Down,       // Key: Page Down
      XK_Page_Up,         // Key: Page Up
      XK_Home,            // Key: Home
      XK_End,             // Key: End
      XK_Caps_Lock,       // Key: Caps Lock
      XK_Scroll_Lock,     // Key: Scroll Lock
      XK_Num_Lock,        // Key: Num Lock
      XK_Print,           // Key: Print
      XK_Pause,           // Key: Pause
      XK_F1,              // Key: F1
      XK_F2,              // Key: F2
      XK_F3,              // Key: F3
      XK_F4,              // Key: F4
      XK_F5,              // Key: F5
      XK_F6,              // Key: F6
      XK_F7,              // Key: F7
      XK_F8,              // Key: F8
      XK_F9,              // Key: F9
      XK_F10,             // Key: F10
      XK_F11,             // Key: F11
      XK_F12,             // Key: F12
      XK_Shift_L,         // Key: Left Shift
      XK_Control_L,       // Key: Left Control
      XK_Alt_L,           // Key: Left Alt
      XK_Super_L,         // Key: Left Super
      XK_Shift_R,         // Key: Right Shift
      XK_Control_R,       // Key: Right Control
      XK_Alt_R,           // Key: Right Alt
      XK_Super_R,         // Key: Right Super
      XK_Menu,            // Key: Menu
      XK_KP_0,            // Key: 0
      XK_KP_1,            // Key: 1
      XK_KP_2,            // Key: 2
      XK_KP_3,            // Key: 3
      XK_KP_4,            // Key: 4
      XK_KP_5,            // Key: 5
      XK_KP_6,            // Key: 6
      XK_KP_7,            // Key: 7
      XK_KP_8,            // Key: 8
      XK_KP_9,            // Key: 9
      XK_KP_Decimal,      // Key: Decimal
      XK_KP_Divide,       // Key: Divide
      XK_KP_Multiply,     // Key: Multiply
      XK_KP_Subtract,     // Key: Subtract
      XK_KP_Add,          // Key: Add
      XK_KP_Enter,        // Key: Enter
      XK_KP_Equal,        // Key: Equal
      MOUSE_LEFT_CODE,    // Mouse Left
      MOUSE_MIDDLE_CODE,  // Mouse Middle
      MOUSE_RIGHT_CODE,   // Mouse Right
      MOUSE_FORWARD_CODE, // Mouse Forward
      MOUSE_BACKWARD_CODE // Mouse Backward
  };

  for (Int32 c = 0; c < KEY_MAX; c++) {
    if (keySymCode[c] == keySym) {
      GEngine.inputApi.currentKeys[c] = bIsPressed;
      return;
    }
  }
}

Bool ApiX11Init(IWindowApi* WindowApi) {
  SLibX11 = EngineLoadModule("libX11.so");
  if (SLibX11 == NULL) {
    return false;
  }
  EngineLoadApi(SLibX11, &SApiX11, SApiX11Name, false);
  WindowApi->OnWindowCreate = &ApiX11WindowCreate;
  WindowApi->OnWindowUpdate = &ApiX11WindowUpdate;
  WindowApi->OnWindowDestroy = &ApiX11WindowDestroy;
  WindowApi->OnWindowFullscreen = &ApiX11WindowFullscreen;
  WindowApi->OnWindowShowCursor = &ApiX11WindowShowCursor;
  WindowApi->OnWindowSetCursorPos = &ApiX11SetCursorPos;
  GT_LOG(LOG_INFO, "API:X11 Initialized");
  return true;
}
#endif // PLATFORM_LINUX

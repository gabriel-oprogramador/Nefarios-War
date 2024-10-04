#include "CoreMinimal.h"
#ifdef PLATFORM_LINUX
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "GT/Engine.h"
#include "GT/Platform.h"
#include "GT/Renderer.h"

static void* SLibX11 = NULL;

static cstring SApiX11Name[] = {
    "XDisplayWidth",               //
    "XDisplayHeight",              //
    "XGetWindowAttributes",        //
    "XScreenNumberOfScreen",       //
    "XResizeWindow",               //
    "XGetInputFocus",              //
    "XGetVisualInfo",              //
    "XCreateColormap",             //
    "XMatchVisualInfo",            //
    "XWarpPointer",                //
    "XUndefineCursor",             //
    "XDisplayKeycodes",            //
    "XGetKeyboardMapping",         //
    "XFree",                       //
    "XCreateBitmapFromData",       //
    "XCreatePixmapCursor",         //
    "XFreePixmap",                 //
    "XCreateFontCursor",           //
    "XDefineCursor",               //
    "XFreeCursor",                 //
    "XkbSetDetectableAutoRepeat",  //
    "XInternAtom",                 //
    "XSetWMProtocols",             //
    "XStoreName",                  //
    "XOpenDisplay",                //
    "XCloseDisplay",               //
    "XMapWindow",                  //
    "XUnmapWindow",                //
    "XSelectInput",                //
    "XNextEvent",                  //
    "XSendEvent",                  //
    "XPending",                    //
    "XSetWMNormalHints",           //
    "XAllocSizeHints",             //
    "XLookupKeysym",               //
    "XDestroyWindow",              //
    "XCreateWindow",               //
    "XCreateSimpleWindow",         //
    "\0"                           //
};

// clang-format off
static struct {
  int (*XDisplayWidth)(Display *, int);
  int (*XDisplayHeight)(Display *, int);
  int (*XGetWindowAttributes)(Display *, Window, XWindowAttributes *);
  int (*XScreenNumberOfScreen)(Screen *);
  int (*XResizeWindow)(Display *, Window, unsigned int, unsigned int);
  int (*XGetInputFocus)(Display *, Window *, int *);
  XVisualInfo* (*XGetVisualInfo)(Display *, long, XVisualInfo *, int *);
  Colormap (*XCreateColormap)(Display *, Window, Visual *, int);
  int (*XMatchVisualInfo)(Display *, int, int, int, XVisualInfo *);
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
  Window (*XCreateWindow)(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual *, unsigned long, XSetWindowAttributes *);
  Window (*XCreateSimpleWindow)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
} SApiX11;
// clang-format on

static struct {
  Window win;
  Display* dpy;
  int scr;
  Atom wmProtocols;
  Atom wmDeleteWindow;
  XSizeHints sizeHints;
} SWindowInfo;

static struct {
  KeySym* map;
  int32 keyCodeMin;
  int32 keyCodeMax;
  int32 keyCodeCount;
  int32 keySymPerkeyCode;
} SKeysMapping;

static void InternalUpdateMonitor();
static void InternalUpdateWindowTitle();
static void InternalGetKeybordMapping();
static void InternalUpdateKey(KeyCode Code, bool bIsButton, bool bIsPressed);

void ApiX11CreateWindow(int32 Width, int32 Height, cstring Title, uint32 Mode) {
  int32 keyMask = KeyPressMask | KeyReleaseMask;
  int32 buttonMask = ButtonPressMask | ButtonReleaseMask;
  int32 otherMask = FocusChangeMask | StructureNotifyMask | PointerMotionMask;
  int32 mask = keyMask | buttonMask | otherMask;

  Display* dpy = SApiX11.XOpenDisplay(NULL);
  int scr = DefaultScreen(dpy);
  XSetWindowAttributes attribs = {0};
  Window win;

  XVisualInfo visualInfo;
  Colormap colorMap;

  if(!SApiX11.XMatchVisualInfo(dpy, scr, 24, TrueColor, &visualInfo)) {
    GT_LOG(LOG_ALERT, "Unsupperted Alpha channel");
  }

  colorMap = SApiX11.XCreateColormap(dpy, RootWindow(dpy, visualInfo.screen), visualInfo.visual, AllocNone);

  attribs.colormap = colorMap;
  attribs.event_mask = mask;
  attribs.background_pixel = 0xFFFF00;
  attribs.border_pixel = 0;
  attribs.override_redirect = false;

  win = SApiX11.XCreateWindow(dpy, RootWindow(dpy, scr),
      10, 10, Width, Height, 1,
      visualInfo.depth,
      InputOutput,
      visualInfo.visual,
      CWColormap | CWBackPixel | CWOverrideRedirect | CWBorderPixel | CWEventMask,
      &attribs);

  SApiX11.XStoreName(dpy, win, Title);

  SApiX11.XMapWindow(dpy, win);
  SApiX11.XkbSetDetectableAutoRepeat(dpy, true, NULL);
  SWindowInfo.wmProtocols = SApiX11.XInternAtom(dpy, "WM_PROTOCOLS", false);
  SWindowInfo.wmDeleteWindow = SApiX11.XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  SApiX11.XSetWMProtocols(dpy, win, &SWindowInfo.wmDeleteWindow, true);

  SWindowInfo.scr = scr;
  SWindowInfo.dpy = dpy;
  SWindowInfo.win = win;

  GEngine.mainWindow.width = Width;
  GEngine.mainWindow.height = Height;
  GEngine.mainWindow.title = Title;
  GEngine.mainWindow.bFullscreen = false;
  GEngine.mainWindow.bShowCursor = true;
  GEngine.mainWindow.bCursorCaptured = false;
  GEngine.mainWindow.bShouldClose = false;
  GEngine.mainWindow.pWindow = (void*)win;
  GEngine.mainWindow.pDevice = (void*)dpy;
  GEngine.mainWindow.pContext = NULL;
  GEngine.windowApi.OnSetMode(Mode);

  InternalGetKeybordMapping();
}

void ApiX11DestroyWindow() {
  SApiX11.XUnmapWindow(SWindowInfo.dpy, SWindowInfo.win);
  SApiX11.XDestroyWindow(SWindowInfo.dpy, SWindowInfo.win);
  SApiX11.XCloseDisplay(SWindowInfo.dpy);
  GEngine.mainWindow.pWindow = NULL;
  GEngine.mainWindow.pDevice = NULL;
  GEngine.mainWindow.bShouldClose = true;
}

void ApiX11UpdateWindow() {
  XEvent event;
  static bool bCursorLocked = false;

#ifdef DEBUG_MODE
  InternalUpdateWindowTitle();
#endif  // DEBUG_MODE

  PMemCopy(&GEngine.inputApi.previousKeys, &GEngine.inputApi.currentKeys, sizeof(GEngine.inputApi.previousKeys));

  while(SApiX11.XPending(SWindowInfo.dpy)) {
    SApiX11.XNextEvent(SWindowInfo.dpy, &event);
    switch(event.type) {
      case FocusIn: {
        if(bCursorLocked) {
          GEngine.mainWindow.bCursorCaptured = true;
        }
      } break;
      case FocusOut: {
        if(event.xfocus.detail == NotifyNonlinear || event.xfocus.detail == NotifyNonlinearVirtual) {
          bCursorLocked = GEngine.mainWindow.bCursorCaptured;
          GEngine.mainWindow.bCursorCaptured = false;
        }
      } break;
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
        int32 width = event.xconfigurerequest.width;
        int32 height = event.xconfigurerequest.height;
        GEngine.mainWindow.width = width;
        GEngine.mainWindow.height = height;
        FRect rect = {0, 0, width, height};
        FSetViewport(rect);
        InternalUpdateMonitor();
      } break;
      case MotionNotify: {
        float posX = event.xmotion.x;
        float posY = event.xmotion.y;
        posX = (posX > GEngine.mainWindow.width) ? GEngine.mainWindow.width : posX;
        posY = (posY > GEngine.mainWindow.width) ? GEngine.mainWindow.height : posY;
        GEngine.inputApi.mousePosition[0] = posX;
        GEngine.inputApi.mousePosition[1] = posY;
      } break;
      case ClientMessage: {
        if(event.xclient.message_type == SWindowInfo.wmProtocols) {
          if(event.xclient.data.l[0] == (int32)SWindowInfo.wmDeleteWindow) {
            FEngineShutdown();
          }
        }
      } break;
    }
  }
}

bool ApiX11IsFocused() {
  Window wFocus;
  SApiX11.XGetInputFocus(SWindowInfo.dpy, &wFocus, NULL);
  return (wFocus == (Window)GEngine.mainWindow.pWindow);
}

void ApiX11SetWindowSize(int32 Width, int32 Height) {
  SApiX11.XResizeWindow(SWindowInfo.dpy, SWindowInfo.win, Width, Height);

  if(SWindowInfo.sizeHints.flags & (PMinSize | PMaxSize)) {
    SWindowInfo.sizeHints.min_width = Width;
    SWindowInfo.sizeHints.max_width = Width;
    SWindowInfo.sizeHints.min_height = Height;
    SWindowInfo.sizeHints.max_height = Height;
    SApiX11.XSetWMNormalHints(SWindowInfo.dpy, SWindowInfo.win, &SWindowInfo.sizeHints);
  }
}

void ApiX11SetWindowMode(uint32 Mode) {
  int32 width = GEngine.mainWindow.width;
  int32 height = GEngine.mainWindow.height;

  switch(Mode) {
    case WINDOW_MODE_FIXED: {
      SWindowInfo.sizeHints.flags = PMinSize | PMaxSize;
      SWindowInfo.sizeHints.min_width = width;
      SWindowInfo.sizeHints.max_width = width;
      SWindowInfo.sizeHints.min_height = height;
      SWindowInfo.sizeHints.max_height = height;
      SApiX11.XSetWMNormalHints(SWindowInfo.dpy, SWindowInfo.win, &SWindowInfo.sizeHints);
    } break;
    case WINDOW_MODE_RESIZABLE: {
      SWindowInfo.sizeHints.flags = 0;
      SApiX11.XSetWMNormalHints(SWindowInfo.dpy, SWindowInfo.win, &SWindowInfo.sizeHints);
    } break;
  }
  GEngine.mainWindow.windowMode = Mode;
}

void ApiX11SetFullscreen(bool bFullscreen) {
  XEvent event;

  if(GEngine.mainWindow.bFullscreen == bFullscreen) {
    return;
  }
  if(GEngine.mainWindow.windowMode == WINDOW_MODE_FIXED) {
    SWindowInfo.sizeHints.flags = (bFullscreen) ? 0 : PMinSize | PMaxSize;
    SApiX11.XSetWMNormalHints(SWindowInfo.dpy, SWindowInfo.win, &SWindowInfo.sizeHints);
  }

  GEngine.mainWindow.bFullscreen = bFullscreen;
  Atom wmState = SApiX11.XInternAtom(SWindowInfo.dpy, "_NET_WM_STATE", false);
  Atom wmFullscreen = SApiX11.XInternAtom(SWindowInfo.dpy, "_NET_WM_STATE_FULLSCREEN", false);
  memset(&event, 0, 1);
  event.type = ClientMessage;
  event.xclient.window = SWindowInfo.win;
  event.xclient.message_type = wmState;
  event.xclient.format = 32;
  event.xclient.data.l[0] = bFullscreen;
  event.xclient.data.l[1] = wmFullscreen;
  event.xclient.data.l[2] = 0;
  SApiX11.XSendEvent(SWindowInfo.dpy, DefaultRootWindow(SWindowInfo.dpy), false, StructureNotifyMask, &event);
}

void ApiX11SetCursorPos(uint32 X, uint32 Y) {
  SApiX11.XWarpPointer(SWindowInfo.dpy, None, SWindowInfo.win, 0, 0, 0, 0, X, Y);
}

void ApiX11ShowCursor(bool bShow) {
  if(bShow) {
    SApiX11.XUndefineCursor(SWindowInfo.dpy, SWindowInfo.win);
  } else {
    Pixmap noData;
    Cursor invisibleCursor;
    XColor black;
    static char noDataBits[] = {0};
    noData = SApiX11.XCreateBitmapFromData(SWindowInfo.dpy, SWindowInfo.win, noDataBits, 1, 1);
    black.red = black.green = black.blue = 0;
    invisibleCursor = SApiX11.XCreatePixmapCursor(SWindowInfo.dpy, noData, noData, &black, &black, 0, 0);
    SApiX11.XDefineCursor(SWindowInfo.dpy, SWindowInfo.win, invisibleCursor);
  }
}

// Internal Functions
static void InternalUpdateMonitor() {
  XWindowAttributes attibs;

  if(SApiX11.XGetWindowAttributes(SWindowInfo.dpy, SWindowInfo.win, &attibs)) {
    Screen* screen = attibs.screen;
    SWindowInfo.scr = SApiX11.XScreenNumberOfScreen(screen);
    GEngine.mainWindow.monitorWidth = SApiX11.XDisplayWidth(SWindowInfo.dpy, SWindowInfo.scr);
    GEngine.mainWindow.monitorHeight = SApiX11.XDisplayHeight(SWindowInfo.dpy, SWindowInfo.scr);
  }
}

static void InternalUpdateWindowTitle() {
  char buffer[BUFFER_SMALL] = "";
  cstring title = GEngine.mainWindow.title;
  double deltaTime = GEngine.timerApi.deltaTime;
  uint32 frameRate = GEngine.timerApi.frameRate;
  snprintf(buffer, BUFFER_SMALL, "%s (Debug Mode) => FPS:%u | MS:%f", title, frameRate, deltaTime);
  SApiX11.XStoreName(SWindowInfo.dpy, SWindowInfo.win, buffer);
}

static void InternalGetKeybordMapping() {
  if(SKeysMapping.map != NULL){
    return;
  }
  SApiX11.XDisplayKeycodes(SWindowInfo.dpy, &SKeysMapping.keyCodeMin, &SKeysMapping.keyCodeMax);
  SKeysMapping.keyCodeCount = SKeysMapping.keyCodeMax - SKeysMapping.keyCodeMin + 1;
  SKeysMapping.map = SApiX11.XGetKeyboardMapping(SWindowInfo.dpy, SKeysMapping.keyCodeMin, SKeysMapping.keyCodeCount, &SKeysMapping.keySymPerkeyCode);
}

static void InternalUpdateKey(KeyCode Code, bool bIsButton, bool bIsPressed) {
  KeySym keySym = 0;
  if(bIsButton) {
    keySym = Code;
  } else {
    int32 index = (Code - SKeysMapping.keyCodeMin) * SKeysMapping.keySymPerkeyCode;
    keySym = SKeysMapping.map[index];
    keySym = (keySym >= XK_a && keySym <= XK_z) ? keySym - (XK_a - XK_A) : keySym;
  }

  static uint16 keySymCode[] = {
      XK_apostrophe,       // Key: '
      XK_comma,            // Key: ,
      XK_minus,            // Key: -
      XK_period,           // Key: .
      XK_slash,            // Key: /
      XK_0,                // Key: 0
      XK_1,                // Key: 1
      XK_2,                // Key: 2
      XK_3,                // Key: 3
      XK_4,                // Key: 4
      XK_5,                // Key: 5
      XK_6,                // Key: 6
      XK_7,                // Key: 7
      XK_8,                // Key: 8
      XK_9,                // Key: 9
      XK_semicolon,        // Key: ;
      XK_equal,            // Key: =
      XK_A,                // Key: A
      XK_B,                // Key: B
      XK_C,                // Key: C
      XK_D,                // Key: D
      XK_E,                // Key: E
      XK_F,                // Key: F
      XK_G,                // Key: G
      XK_H,                // Key: H
      XK_I,                // Key: I
      XK_J,                // Key: J
      XK_K,                // Key: K
      XK_L,                // Key: L
      XK_M,                // Key: M
      XK_N,                // Key: N
      XK_O,                // Key: O
      XK_P,                // Key: P
      XK_Q,                // Key: Q
      XK_R,                // Key: R
      XK_S,                // Key: S
      XK_T,                // Key: T
      XK_U,                // Key: U
      XK_V,                // Key: V
      XK_W,                // Key: W
      XK_X,                // Key: X
      XK_Y,                // Key: Y
      XK_Z,                // Key: Z
      XK_bracketleft,      // Key: [{
      XK_backslash,        // Key: '\'
      XK_bracketright,     // Key: ]}
      XK_grave,            // Key: `
      XK_space,            // Key: Space
      XK_Escape,           // Key: Escape
      XK_Return,           // Key: Enter
      XK_Tab,              // Key: Tab
      XK_BackSpace,        // Key: Backspace
      XK_Insert,           // Key: Insert
      XK_Delete,           // Key: Delete
      XK_Right,            // Key: Right
      XK_Left,             // Key: Left
      XK_Down,             // Key: Down
      XK_Up,               // Key: Up
      XK_Page_Down,        // Key: Page Down
      XK_Page_Up,          // Key: Page Up
      XK_Home,             // Key: Home
      XK_End,              // Key: End
      XK_Caps_Lock,        // Key: Caps Lock
      XK_Scroll_Lock,      // Key: Scroll Lock
      XK_Num_Lock,         // Key: Num Lock
      XK_Print,            // Key: Print
      XK_Pause,            // Key: Pause
      XK_F1,               // Key: F1
      XK_F2,               // Key: F2
      XK_F3,               // Key: F3
      XK_F4,               // Key: F4
      XK_F5,               // Key: F5
      XK_F6,               // Key: F6
      XK_F7,               // Key: F7
      XK_F8,               // Key: F8
      XK_F9,               // Key: F9
      XK_F10,              // Key: F10
      XK_F11,              // Key: F11
      XK_F12,              // Key: F12
      XK_Shift_L,          // Key: Left Shift
      XK_Control_L,        // Key: Left Control
      XK_Alt_L,            // Key: Left Alt
      XK_Super_L,          // Key: Left Super
      XK_Shift_R,          // Key: Right Shift
      XK_Control_R,        // Key: Right Control
      XK_Alt_R,            // Key: Right Alt
      XK_Super_R,          // Key: Right Super
      XK_Menu,             // Key: Menu
      XK_KP_0,             // Key: 0
      XK_KP_1,             // Key: 1
      XK_KP_2,             // Key: 2
      XK_KP_3,             // Key: 3
      XK_KP_4,             // Key: 4
      XK_KP_5,             // Key: 5
      XK_KP_6,             // Key: 6
      XK_KP_7,             // Key: 7
      XK_KP_8,             // Key: 8
      XK_KP_9,             // Key: 9
      XK_KP_Decimal,       // Key: Decimal
      XK_KP_Divide,        // Key: Divide
      XK_KP_Multiply,      // Key: Multiply
      XK_KP_Subtract,      // Key: Subtract
      XK_KP_Add,           // Key: Add
      XK_KP_Enter,         // Key: Enter
      XK_KP_Equal,         // Key: Equal
      MOUSE_LEFT_CODE,     // Mouse Left
      MOUSE_MIDDLE_CODE,   // Mouse Middle
      MOUSE_RIGHT_CODE,    // Mouse Right
      MOUSE_FORWARD_CODE,  // Mouse Forward
      MOUSE_BACKWARD_CODE  // Mouse Backward
  };

  for(int32 c = 0; c < KEY_MAX; c++) {
    if(keySymCode[c] == keySym) {
      GEngine.inputApi.currentKeys[c] = bIsPressed;
      return;
    }
  }
}

bool ApiX11Init(IWindowApi* Api) {
  SLibX11 = PModuleLoad("libX11.so");
  if(SLibX11 == NULL) {
    return false;
  }

  PModuleLoadApi(SLibX11, &SApiX11, SApiX11Name, false);
  Api->OnCreate = &ApiX11CreateWindow;
  Api->OnDestroy = &ApiX11DestroyWindow;
  Api->OnUpdate = &ApiX11UpdateWindow;
  Api->OnIsFocused = &ApiX11IsFocused;
  Api->OnSetSize = &ApiX11SetWindowSize;
  Api->OnSetMode = &ApiX11SetWindowMode;
  Api->OnSetFullscreen = &ApiX11SetFullscreen;
  Api->OnSetCursorPos = &ApiX11SetCursorPos;
  Api->OnShowCursor = &ApiX11ShowCursor;

  GT_LOG(LOG_INFO, "API-X11: Initialized");
  return true;
}

#endif  // PLATFORM_LINUX

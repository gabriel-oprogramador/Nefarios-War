#include "CoreMinimal.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include "GT/Platform.h"
#include "GT/Engine.h"
#include "GT/Renderer.h"

static void* SLibUser = NULL;
static const uint32 CODE_WINDOW_CLOSE = WM_USER + 1;
static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static void InternalSetWindowTitle();
static void InternalUpdateKey(uint64 KeyCode, bool bIsPressed);

static const char* SLibUserNames[] = {
    "GetMonitorInfoA",
    "MonitorFromWindow",
    "GetForegroundWindow",
    "ReleaseDC",
    "EnumDisplaySettingsA",
    "ChangeDisplaySettingsA",
    "SetCursorPos",
    "ClientToScreen",
    "ShowWindow",
    "UpdateWindow",
    "GetWindowPlacement",
    "SetWindowPlacement",
    "SetWindowTextA",
    "ShowCursor",
    "RegisterClassExA",
    "DefWindowProcA",
    "LoadIconA",
    "LoadCursorA",
    "PeekMessageA",
    "TranslateMessage",
    "DispatchMessageA",
    "PostMessageA",
    "GetSystemMetrics",
    "GetWindowLongPtrA",
    "SetWindowLongPtrA",
    "SetWindowPos",
    "GetWindowRect",
    "GetClientRect",
    "AdjustWindowRect",
    "DestroyWindow",
    "GetClassInfoExA",
    "GetDC",
    "CreateWindowExA",
    "\0"};

// clang-format off
static struct {
  int (*GetMonitorInfoA)(HMONITOR hMonitor, LPMONITORINFO lpmi);
  HMONITOR (*MonitorFromWindow)(HWND hwnd, DWORD dwFlags);
  HWND (*GetForegroundWindow)();
  int (*ReleaseDC)(HWND hWnd, HDC hDC);
  BOOL (*EnumDisplaySettingsA)(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODE *lpDevMode);
  LONG (*ChangeDisplaySettingsA)(DEVMODE *lpDevMode, DWORD dwFlags);
  BOOL (*SetCursorPos)(int X, int Y);
  BOOL (*ClientToScreen)(HWND hWnd, LPPOINT lpPoint);
  BOOL (*ShowWindow)(HWND hWnd, int nCmdShow);
  BOOL (*UpdateWindow)(HWND hWnd);
  BOOL (*GetWindowPlacement)(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
  BOOL (*SetWindowPlacement)(HWND hWnd, const WINDOWPLACEMENT *lpwndpl);
  BOOL (*SetWindowTextA) (HWND hWnd, LPCSTR lpcstring);
  int (*ShowCursor)(BOOL bShow);
  ATOM (*RegisterClassExA) (const WNDCLASSEXA* unnamedParam1);
  LRESULT (*DefWindowProcA) (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
  HICON (*LoadIconA) (HINSTANCE hInstance, LPCSTR lpIconName);
  HCURSOR (*LoadCursorA) (HINSTANCE hInstance, LPCSTR lpCursorName);
  BOOL (*PeekMessageA) (LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
  BOOL (*TranslateMessage) (const MSG* lpMsg);
  LRESULT (*DispatchMessageA) (const MSG* lpMsg);
  BOOL (*PostMessageA) (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
  int (*GetSystemMetrics)(int nIndex);
  LONG_PTR (*GetWindowLongPtrA)(HWND hWnd, int nIndex);
  LONG_PTR (*SetWindowLongPtrA) (HWND hWnd, int nIndex, LONG_PTR dwNewLong);
  BOOL (*SetWindowPos) (HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
  BOOL (*GetWindowRect) (HWND hWnd, LPRECT lpRect);
  BOOL (*GetClientRect) (HWND hWnd, LPRECT lpRect);
  BOOL (*AdjustWindowRect) (LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
  BOOL (*DestroyWindow) (HWND hWnd);
  BOOL(*GetClassInfoExA) (HINSTANCE hInstance, LPCSTR lpszClass, LPWNDCLASSEXA lpwcx);
  HDC (*GetDC) (HWND hWnd);
  HWND (*CreateWindowExA) (DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
} SApiUser;
// clang-format on

void ApiWin32CreateWindow(int32 Width, int32 Height, cstring Title, uint32 Mode) {
  cstring className = "GameWindow";
  HINSTANCE hInstance = GetModuleHandleA(NULL);
  uint32 style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  WNDCLASSEXA wc = {0};

  if(!SApiUser.GetClassInfoExA(hInstance, className, &wc)) {
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = &InternalWinProc;
    wc.hInstance = hInstance;
    wc.hIcon = SApiUser.LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = SApiUser.LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = className;

    if(!SApiUser.RegisterClassExA(&wc)) {
      GT_LOG(LOG_FATAL, "API-WIN32: Not register class window");
      exit(1);  // Exit Program
    }
  }

  HWND hWin = SApiUser.CreateWindowExA(0, className, Title, style, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, hInstance, NULL);
  if(hWin == NULL) {
    GT_LOG(LOG_FATAL, "API-WIN32: Not create game window");
    exit(1);  // Exit Program
  }

  GEngine.mainWindow.width = Width;
  GEngine.mainWindow.height = Height;
  GEngine.mainWindow.title = Title;
  GEngine.mainWindow.bShouldClose = false;
  GEngine.mainWindow.bFullscreen = false;
  GEngine.mainWindow.bShowCursor = true;
  GEngine.mainWindow.bCursorCaptured = false;
  GEngine.mainWindow.pWindow = (void*)hWin;
  GEngine.mainWindow.pDevice = (void*)SApiUser.GetDC(hWin);
  GEngine.mainWindow.pContext = NULL;
  GEngine.windowApi.OnSetMode(Mode);
  GEngine.windowApi.OnSetSize(Width, Height);
}

void ApiWin32UpdateWindow() {
  MSG msg;
#ifdef DEBUG_MODE
  InternalSetWindowTitle();
#endif  // DEBUG_MODE

  PMemCopy(GEngine.inputApi.previousKeys, GEngine.inputApi.currentKeys, KEY_MAX);
  while(SApiUser.PeekMessageA(&msg, (HWND)GEngine.mainWindow.pWindow, 0, 0, PM_REMOVE)) {
    if(msg.message == CODE_WINDOW_CLOSE) {
      FEngineShutdown();
      return;
    }
    SApiUser.TranslateMessage(&msg);
    SApiUser.DispatchMessageA(&msg);
  }
}

void ApiWin32DestroyWindow() {
  HWND win = (HWND)GEngine.mainWindow.pWindow;
  HDC dc = (HDC)GEngine.mainWindow.pDevice;
  SApiUser.ReleaseDC(win, dc);
  SApiUser.DestroyWindow(win);
  GEngine.mainWindow.pWindow = NULL;
  GEngine.mainWindow.pDevice = NULL;
  GEngine.mainWindow.bShouldClose = true;
}

bool ApiWin32WindowIsFocused() {
  bool retVal = (void*)SApiUser.GetForegroundWindow() == GEngine.mainWindow.pWindow;
  return retVal;
}

void ApiWin32SetWindowSize(int32 Width, int32 Height) {
  HWND win = GEngine.mainWindow.pWindow;
  RECT winRect = {(LONG)0, (LONG)0, (LONG)Width, (LONG)Height};
  LONG_PTR style = SApiUser.GetWindowLongPtrA(win, GWL_STYLE);

  if(!SApiUser.AdjustWindowRect(&winRect, style, FALSE)) {
    GT_LOG(LOG_ERROR, "API-WIN32: No adjust window rect");
  }

  int32 winWidth = winRect.right - winRect.left;
  int32 winHeight = winRect.bottom - winRect.top;

  SApiUser.SetWindowPos(win, HWND_TOP, 0, 0, winWidth, winHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void ApiWin32SetWindowMode(uint32 WindowMode) {
  HWND win = (HWND)GEngine.mainWindow.pWindow;
  LONG_PTR flag = WS_VISIBLE;

  switch(WindowMode) {
    case WINDOW_MODE_FIXED: {
      flag |= WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    } break;
    case WINDOW_MODE_RESIZABLE: {
      flag |= WS_OVERLAPPEDWINDOW;
      SApiUser.SetWindowLongPtrA(win, GWL_STYLE, flag);
    } break;
  }
  SApiUser.SetWindowLongPtrA(win, GWL_STYLE, flag);
  GEngine.windowApi.OnSetSize(GEngine.mainWindow.width, GEngine.mainWindow.height);
  GEngine.mainWindow.windowMode = WindowMode;
}

void ApiWin32SetFullscreen(bool bFullscreen) {
  if(GEngine.mainWindow.bFullscreen == bFullscreen) {
    return;
  }
  GEngine.mainWindow.bFullscreen = bFullscreen;
  HWND win = GEngine.mainWindow.pWindow;
  static LONG_PTR style;
  static WINDOWPLACEMENT placement = {sizeof(WINDOWPLACEMENT)};

  if(bFullscreen) {
    DEVMODE dm;
    memset(&dm, 0, sizeof(DEVMODE));
    SApiUser.EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm);
    SApiUser.ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN);
    SApiUser.GetWindowPlacement(win, &placement);
    style = SApiUser.GetWindowLongPtrA(win, GWL_STYLE);

    SApiUser.SetWindowLongPtrA(win, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    uint32 mask = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
    SApiUser.SetWindowPos(win, NULL, 0, 0, dm.dmPelsWidth, dm.dmPelsHeight, mask);

  } else {
    SApiUser.SetWindowLongPtrA(win, GWL_STYLE, style);
    SApiUser.SetWindowPlacement(win, &placement);
    SApiUser.ChangeDisplaySettingsA(NULL, 0);
  }
}

void ApiWin32SetCursorPos(uint32 X, uint32 Y) {
  POINT point = {X, Y};
  SApiUser.ClientToScreen((HWND)GEngine.mainWindow.pWindow, &point);
  SApiUser.SetCursorPos(point.x, point.y);
}

void ApiWin32ShowCursor(bool bShow) {
  SApiUser.ShowCursor(bShow);
}

// Internal Functions
static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  static bool bCursorLocked = false;
  switch(Msg) {
    case WM_CLOSE: {
      SApiUser.PostMessageA(hWnd, CODE_WINDOW_CLOSE, 0, 0);
    } break;

    case WM_SETFOCUS: {
      if(bCursorLocked) {
        GEngine.mainWindow.bCursorCaptured = true;
      }
    } break;
    case WM_KILLFOCUS: {
      bCursorLocked = GEngine.mainWindow.bCursorCaptured;
      GEngine.mainWindow.bCursorCaptured = false;
    } break;

    case WM_SIZE: {
      GEngine.mainWindow.width = LOWORD(lParam);
      GEngine.mainWindow.height = HIWORD(lParam);
      if(GEngine.mainWindow.pContext != NULL) {
        FRect viewport = {0, 0, LOWORD(lParam), HIWORD(lParam)};
        FSetViewport(viewport);
      }
      HMONITOR monitor = SApiUser.MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
      MONITORINFO info;
      info.cbSize = sizeof(MONITORINFO);
      if(SApiUser.GetMonitorInfoA(monitor, &info)) {
        GEngine.mainWindow.monitorWidth = info.rcMonitor.right - info.rcMonitor.left;
        GEngine.mainWindow.monitorHeight = info.rcMonitor.bottom - info.rcMonitor.top;
      }
    } break;

    case WM_KEYDOWN: {
      InternalUpdateKey(wParam, true);
    } break;
    case WM_KEYUP: {
      InternalUpdateKey(wParam, false);
    } break;
    case WM_LBUTTONDOWN: {
      InternalUpdateKey(MOUSE_LEFT_CODE, true);
    } break;
    case WM_LBUTTONUP: {
      InternalUpdateKey(MOUSE_LEFT_CODE, false);
    } break;

    case WM_MBUTTONDOWN: {
      InternalUpdateKey(MOUSE_MIDDLE_CODE, true);
    } break;
    case WM_MBUTTONUP: {
      InternalUpdateKey(MOUSE_MIDDLE_CODE, false);
    } break;

    case WM_RBUTTONDOWN: {
      InternalUpdateKey(MOUSE_RIGHT_CODE, true);
    } break;
    case WM_RBUTTONUP: {
      InternalUpdateKey(MOUSE_RIGHT_CODE, false);
    } break;

    case WM_MOUSEWHEEL: {
      int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
      uint32 key = (delta > 0) ? MOUSE_FORWARD_CODE : MOUSE_BACKWARD_CODE;
      InternalUpdateKey(key, true);
    } break;

    case WM_MOUSEMOVE: {
      float posX = (float)GET_X_LPARAM(lParam);
      float posY = (float)GET_Y_LPARAM(lParam);
      GEngine.inputApi.mousePosition[0] = (posX < 0) ? 0.f : posX;
      GEngine.inputApi.mousePosition[1] = (posX < 0) ? 0.f : posY;
    } break;

    default: {
      return SApiUser.DefWindowProcA(hWnd, Msg, wParam, lParam);
    }
  }

  return 0;
}

static void InternalSetWindowTitle() {
  static char buffer[BUFFER_SMALL] = "";
  int32 frameRate = GEngine.timerApi.frameRate;
  float deltaTime = GEngine.timerApi.deltaTime;
  snprintf(buffer, sizeof(buffer), "%s (Debug Mode) => FPS:%d | MS:%f", GEngine.mainWindow.title, frameRate, deltaTime);
  SApiUser.SetWindowTextA(GEngine.mainWindow.pWindow, buffer);
}

static void InternalUpdateKey(uint64 KeyCode, bool bIsPressed) {
  static uint32 vkCode[] = {
      VK_OEM_7,            // Key: '
      VK_OEM_COMMA,        // Key: ,
      VK_OEM_MINUS,        // Key: -
      VK_OEM_PERIOD,       // Key: .
      VK_OEM_2,            // Key: /
      0x30,                // Key: 0
      0x31,                // Key: 1
      0x32,                // Key: 2
      0x33,                // Key: 3
      0x34,                // Key: 4
      0x35,                // Key: 5
      0x36,                // Key: 6
      0x37,                // Key: 7
      0x38,                // Key: 8
      0x39,                // Key: 9
      VK_OEM_1,            // Key: ;
      VK_OEM_NEC_EQUAL,    // Key: =
      0x41,                // Key: A
      0x42,                // Key: B
      0x43,                // Key: C
      0x44,                // Key: D
      0x45,                // Key: E
      0x46,                // Key: F
      0x47,                // Key: G
      0x48,                // Key: H
      0x49,                // Key: I
      0x4A,                // Key: J
      0x4B,                // Key: K
      0x4C,                // Key: L
      0x4D,                // Key: M
      0x4E,                // Key: N
      0x4F,                // Key: O
      0x50,                // Key: P
      0x51,                // Key: Q
      0x52,                // Key: R
      0x53,                // Key: S
      0x54,                // Key: T
      0x55,                // Key: U
      0x56,                // Key: V
      0x57,                // Key: W
      0x58,                // Key: X
      0x59,                // Key: Y
      0x5A,                // Key: Z
      VK_OEM_4,            // Key: [{
      VK_OEM_102,          // Key: '\'
      VK_OEM_6,            // Key: ]}
      VK_OEM_3,            // Key: `
      VK_SPACE,            // Key: Space
      VK_ESCAPE,           // Key: Escape
      VK_RETURN,           // Key: Enter
      VK_TAB,              // Key: Tab
      VK_BACK,             // Key: Backspace
      VK_INSERT,           // Key: Insert
      VK_DELETE,           // Key: Delete
      VK_RIGHT,            // Key: Right
      VK_LEFT,             // Key: Left
      VK_DOWN,             // Key: Down
      VK_UP,               // Key: Up
      VK_NEXT,             // Key: Page Down
      VK_PRIOR,            // Key: Page Up
      VK_HOME,             // Key: Home
      VK_END,              // Key: End
      VK_CAPITAL,          // Key: Caps Look
      VK_SCROLL,           // Key: Scroll Down
      VK_NUMLOCK,          // Key: Num Lock
      VK_PRINT,            // Key: Print
      VK_PAUSE,            // Key: Pause
      VK_F1,               // Key: F1
      VK_F2,               // Key: F2
      VK_F3,               // Key: F3
      VK_F4,               // Key: F4
      VK_F5,               // Key: F5
      VK_F6,               // Key: F6
      VK_F7,               // Key: F7
      VK_F8,               // Key: F8
      VK_F9,               // Key: F9
      VK_F10,              // Key: F10
      VK_F11,              // Key: F11
      VK_F12,              // Key: F12
      VK_SHIFT,            // Key: Left Shift
      VK_CONTROL,          // Key: Left Control
      VK_MENU,             // Key: Left Alt
      VK_LWIN,             // Key: Left Super
      VK_RSHIFT,           // Key: Left Shift
      VK_RCONTROL,         // Key: Right Control
      VK_RMENU,            // Key: Right Alt
      VK_RWIN,             // Key: Right Super
      VK_MENU,             // Key: Menu
      0x30,                // Key: 0
      0x31,                // Key: 1
      0x32,                // Key: 2
      0x33,                // Key: 3
      0x34,                // Key: 4
      0x35,                // Key: 5
      0x36,                // Key: 6
      0x37,                // Key: 7
      0x38,                // Key: 8
      0x39,                // Key: 9
      VK_DECIMAL,          // Key: de decimal
      VK_DIVIDE,           // Key: de divisão
      VK_MULTIPLY,         // Key: de multiplicação
      VK_SUBTRACT,         // Key: de subtração
      VK_ADD,              // Key: de adição
      VK_RETURN,           // Key: Enter
      VK_OEM_NEC_EQUAL,    // Key: Equal
      MOUSE_LEFT_CODE,     // Mouse Left
      MOUSE_MIDDLE_CODE,   // Mouse Middle
      MOUSE_RIGHT_CODE,    // Mouse Right
      MOUSE_FORWARD_CODE,  // Mouse Forward
      MOUSE_BACKWARD_CODE  // Mouse Backward
  };

  for(int32 c = 0; c < KEY_MAX; c++) {
    if(vkCode[c] == KeyCode) {
      GEngine.inputApi.currentKeys[c] = bIsPressed;
      break;
    }
  }
}

bool ApiWin32Init(IWindowApi* Api) {
  SLibUser = PModuleLoad("user32.dll");
  if(SLibUser == NULL) {
    return false;
  }
  PModuleLoadApi(SLibUser, &SApiUser, SLibUserNames, false);
  Api->OnCreate = &ApiWin32CreateWindow;
  Api->OnUpdate = &ApiWin32UpdateWindow;
  Api->OnDestroy = &ApiWin32DestroyWindow;
  Api->OnIsFocused = &ApiWin32WindowIsFocused;
  Api->OnSetSize = &ApiWin32SetWindowSize;
  Api->OnSetMode = &ApiWin32SetWindowMode;
  Api->OnSetFullscreen = &ApiWin32SetFullscreen;
  Api->OnSetCursorPos = &ApiWin32SetCursorPos;
  Api->OnShowCursor = &ApiWin32ShowCursor;

  GT_LOG(LOG_INFO, "API-WIN32: Initialized");
  return true;
}

#endif  // PLATFORM_WINDOWS

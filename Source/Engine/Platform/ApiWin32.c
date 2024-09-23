#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>

#include "GT/Engine.h"

#define CODE_WINDOW_CLOSE WM_USER + 1

extern HGLRC ApiWglInit(HWND Window, HDC Device, int32 Major, int32 Minor, int32 ColorBits, int32 DepthBits);

static void* SLibUser32 = NULL;

static const char* SLibUser32Names[] = {
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
} SApiUser32;
// clang-format on

static struct {
  WINDOWPLACEMENT placement;
  LONG_PTR style;
  HWND window;
  HGLRC context;
  HDC device;
} SWindow;

static void InternalUpdateWindowTitle();
static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static void InternalUpdateKey(uint64 KeyCode, bool bIsPressed);

// Functions to assist in creating the dummy window for WGL Api.
HWND ApiWin32CreateWindow(int32 Width, int32 Height, cstring Title);
void ApiWin32DestroyWindow(HWND Window);
HDC ApiWin32GetDC(HWND Window);

void ApiWin32WindowCreate(int32 Width, int32 Height, cstring Title) {
  SWindow.window = ApiWin32CreateWindow(Width, Height, Title);
  SWindow.device = ApiWin32GetDC(SWindow.window);
  // TODO: Never use magic numbers!
  SWindow.context = ApiWglInit(SWindow.window, SWindow.device, 3, 3, 32, 24);
  GEngine.windowApi.width = Width;
  GEngine.windowApi.height = Height;
  GEngine.windowApi.title = Title;
  GEngine.windowApi.bFullscreen = false;
  GEngine.windowApi.bShouldClose = false;
  GT_LOG(LOG_INFO, "API:WIN32 Created Window => Width:%d Height:%d Title:%s", Width, Height, Title);
}

void ApiWin32WindowUpdate() {
#ifdef DEBUG_MODE
  InternalUpdateWindowTitle();
#endif  // DEBUG_MODE

  MSG msg;
  memcpy(GEngine.inputApi.previousKeys, GEngine.inputApi.currentKeys, sizeof(GEngine.inputApi.previousKeys));
  InternalUpdateKey(MOUSE_FORWARD_CODE, false);
  InternalUpdateKey(MOUSE_BACKWARD_CODE, false);

  while(SApiUser32.PeekMessageA(&msg, SWindow.window, 0, 0, PM_REMOVE)) {
    if(msg.message == CODE_WINDOW_CLOSE) {
      GEngine.windowApi.bShouldClose = true;
      return;
    }
    SApiUser32.TranslateMessage(&msg);
    SApiUser32.DispatchMessageA(&msg);
  }
}

void ApiWin32WindowDestroy() {
  GT_LOG(LOG_INFO, "API:WIN32 Closed Window");
  ApiWin32DestroyWindow(SWindow.window);
}

static void InternalUpdateWindowTitle() {
  char buffer[BUFFER_SMALL] = "";
  cstring title = GEngine.windowApi.title;
  double deltaTime = GEngine.timerApi.deltaTime;
  uint32 frameRate = GEngine.timerApi.frameRate;
  snprintf(buffer, BUFFER_SMALL, "%s (Debug Mode) => FPS:%u | MS:%f", title, frameRate, deltaTime);
  SApiUser32.SetWindowTextA(SWindow.window, buffer);
}

static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  switch(Msg) {
    case WM_CLOSE: {
      SApiUser32.PostMessageA(hWnd, CODE_WINDOW_CLOSE, 0, 0);
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

    case WM_SIZE: {
      GEngine.windowApi.width = LOWORD(lParam);
      GEngine.windowApi.height = HIWORD(lParam);
      // TODO:Adjust the GL Viewport.
    } break;

    default: {
      return SApiUser32.DefWindowProcA(hWnd, Msg, wParam, lParam);
    }
  }

  return 0;
}

void ApiWin32SetFullscreen(bool bFullscreen) {
  GEngine.windowApi.bFullscreen = bFullscreen;

  if(bFullscreen) {
    DEVMODE dm;
    memset(&dm, 0, sizeof(DEVMODE));
    SApiUser32.EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm);
    SApiUser32.ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN);
    SApiUser32.GetWindowPlacement(SWindow.window, &SWindow.placement);
    SWindow.style = SApiUser32.GetWindowLongPtrA(SWindow.window, GWL_STYLE);

    SApiUser32.SetWindowLongPtrA(SWindow.window, GWL_STYLE, WS_POPUP);
    uint32 mask = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
    SApiUser32.SetWindowPos(SWindow.window, HWND_TOP, 0, 0, dm.dmPelsWidth, dm.dmPelsHeight, mask);
  } else {
    SApiUser32.SetWindowLongPtrA(SWindow.window, GWL_STYLE, SWindow.style);
    SApiUser32.SetWindowPlacement(SWindow.window, &SWindow.placement);
    SApiUser32.ChangeDisplaySettingsA(NULL, 0);
  }
  SApiUser32.ShowWindow(SWindow.window, SW_SHOW);
  SApiUser32.UpdateWindow(SWindow.window);
}

void ApiWin32ShowCursor(bool bShow) {
  if(GEngine.windowApi.bShowCursor == bShow) {
    return;
  }
  GEngine.windowApi.bShowCursor = bShow;
  SApiUser32.ShowCursor(bShow);
}

void ApiWin32SetCursorPos(uint32 X, uint32 Y) {
  RECT windowRect = {0};
  POINT clientTopLeft = {0};

  SApiUser32.GetWindowRect(SWindow.window, &windowRect);
  SApiUser32.ClientToScreen(SWindow.window, &clientTopLeft);

  uint32 borderOffsetX = clientTopLeft.x - windowRect.left;
  uint32 borderOffsetY = clientTopLeft.y - windowRect.top;
  uint32 xScreen = windowRect.left + borderOffsetX + X;
  uint32 yScreen = windowRect.top + borderOffsetY + Y;

  SApiUser32.SetCursorPos(xScreen, yScreen);
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

// Function to assist in creating the dummy window for WGL Api.
HWND ApiWin32CreateWindow(int32 Width, int32 Height, cstring Title) {
  cstring windowClassName = "GameWindow";
  HINSTANCE hInstance = GetModuleHandleA(NULL);
  WNDCLASSEXA wc = {0};

  if(!SApiUser32.GetClassInfoExA(hInstance, windowClassName, &wc)) {
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = &InternalWinProc;
    wc.hInstance = hInstance;
    wc.hIcon = SApiUser32.LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = SApiUser32.LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = windowClassName;

    if(!SApiUser32.RegisterClassExA(&wc)) {
      GT_LOG(LOG_FATAL, "API:WIN32 Not Register Class Window");
      exit(1);  // Exit Program
    }
  }

  RECT winRect = {(LONG)0, (LONG)0, (LONG)Width, (LONG)Height};
  if(!SApiUser32.AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE)) {
    GT_LOG(LOG_ERROR, "API:WIN32 No Adjust Window Rect");
  }

  int32 winWidth = winRect.right - winRect.left;
  int32 winHeight = winRect.bottom - winRect.top;
  uint32 winStyle = WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
  HWND hWin = SApiUser32.CreateWindowExA(0, windowClassName, Title, winStyle, CW_USEDEFAULT, CW_USEDEFAULT, winWidth, winHeight, NULL, NULL, hInstance, NULL);
  if(hWin == NULL) {
    GT_LOG(LOG_FATAL, "API:WIN32 Not Create Game Window");
    exit(1);  // Exit Program
  }

  return hWin;
}

void ApiWin32DestroyWindow(HWND Window) {
  SApiUser32.DestroyWindow(Window);
}

HDC ApiWin32GetDC(HWND Window) {
  return SApiUser32.GetDC(Window);
}

// Setting the Win32 API interface.
bool ApiWin32Init(IWindowApi* WindowApi) {
  SLibUser32 = EngineLoadModule("user32.dll");
  if(SLibUser32 == NULL) {
    return false;
  }
  EngineLoadApi(SLibUser32, &SApiUser32, SLibUser32Names, false);
  WindowApi->OnWindowCreate = &ApiWin32WindowCreate;
  WindowApi->OnWindowUpdate = &ApiWin32WindowUpdate;
  WindowApi->OnWindowDestroy = &ApiWin32WindowDestroy;
  WindowApi->OnWindowFullscreen = &ApiWin32SetFullscreen;
  WindowApi->OnWindowShowCursor = &ApiWin32ShowCursor;
  WindowApi->OnWindowSetCursorPos = &ApiWin32SetCursorPos;
  GT_LOG(LOG_INFO, "API:WIN32 Initialized");
  return true;
}

#endif  // PLATFORM_WINDOWS

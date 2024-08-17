#ifdef PLATFORM_WINDOWS
#include "Engine.h"

#include <windows.h>
#include <windowsx.h>

#define CODE_WINDOW_CLOSE WM_USER + 1

static Void* SLibUser32 = NULL;

static const char* SLibUser32Names[] = {
    "SetWindowTextA",    //
    "ShowCursor",        //
    "RegisterClassExA",  //
    "DefWindowProcA",    //
    "LoadIconA",         //
    "LoadCursorA",       //
    "PeekMessageA",      //
    "TranslateMessage",  //
    "DispatchMessageA",  //
    "PostMessageA",      //
    "GetSystemMetrics",  //
    "SetWindowLongPtrA", //
    "SetWindowPos",      //
    "GetWindowRect",     //
    "GetClientRect",     //
    "AdjustWindowRect",  //
    "DestroyWindow",     //
    "GetClassInfoExA",   //
    "GetDC",             //
    "CreateWindowExA",   //
    "\0"                 //
};

static struct {
  BOOL(*SetWindowTextA)
  (HWND hWnd, LPCSTR lpString);
  int (*ShowCursor)(BOOL bShow);
  ATOM(*RegisterClassExA)
  (const WNDCLASSEXA* unnamedParam1);
  LRESULT(*DefWindowProcA)
  (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
  HICON(*LoadIconA)
  (HINSTANCE hInstance, LPCSTR lpIconName);
  HCURSOR(*LoadCursorA)
  (HINSTANCE hInstance, LPCSTR lpCursorName);
  BOOL(*PeekMessageA)
  (LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
  BOOL(*TranslateMessage)
  (const MSG* lpMsg);
  LRESULT(*DispatchMessageA)
  (const MSG* lpMsg);
  BOOL(*PostMessageA)
  (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
  int (*GetSystemMetrics)(int nIndex);
  LONG_PTR(*SetWindowLongPtrA)
  (HWND hWnd, int nIndex, LONG_PTR dwNewLong);
  BOOL(*SetWindowPos)
  (HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
  BOOL(*GetWindowRect)
  (HWND hWnd, LPRECT lpRect);
  BOOL(*GetClientRect)
  (HWND hWnd, LPRECT lpRect);
  BOOL(*AdjustWindowRect)
  (LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
  BOOL(*DestroyWindow)
  (HWND hWnd);
  BOOL(*GetClassInfoExA)
  (HINSTANCE hInstance, LPCSTR lpszClass, LPWNDCLASSEXA lpwcx);
  HDC(*GetDC)
  (HWND hWnd);
  HWND(*CreateWindowExA)
  (DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
   HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
} SApiUser32;

static struct {
  Int32 lastWidth;
  Int32 lastHeight;
  Int32 lastPosX;
  Int32 lastPosY;
  HWND window;
  HDC device;
} SWindow;

static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static Void InternalUpdateKey(UInt64 KeyCode, Bool bIsPressed);

Void ApiWin32WindowCreate(Int32 Width, Int32 Height, String Title) {
  String windowClassName = "GameWindow";
  HINSTANCE hInstance = GetModuleHandleA(NULL);
  WNDCLASSEXA wc = {0};

  if (!SApiUser32.GetClassInfoExA(hInstance, windowClassName, &wc)) {
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = &InternalWinProc;
    wc.hInstance = hInstance;
    wc.hIcon = SApiUser32.LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = SApiUser32.LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = windowClassName;

    if (!SApiUser32.RegisterClassExA(&wc)) {
      GT_LOG(LOG_FATAL, "API:Win32 Not Register Class Window");
      exit(1); // Exit Program
    }
  }

  RECT winRect = {(LONG)0, (LONG)0, (LONG)Width, (LONG)Height};
  if (!SApiUser32.AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE)) {
    GT_LOG(LOG_ERROR, "API:Win32 No Adjust Window Rect");
  }

  Int32 winWidth = winRect.right - winRect.left;
  Int32 winHeight = winRect.bottom - winRect.top;
  UInt32 winStyle = WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
  HWND hWin = SApiUser32.CreateWindowExA(0, windowClassName, Title, winStyle, CW_USEDEFAULT, CW_USEDEFAULT, winWidth, winHeight, NULL, NULL, hInstance, NULL);
  if (hWin == NULL) {
    GT_LOG(LOG_FATAL, "API:Win32 Not Create Game Window");
    exit(1); // Exit Program
  }

  GEngine.windowApi.width = Width;
  GEngine.windowApi.height = Height;
  GEngine.windowApi.title = Title;
  GEngine.windowApi.bFullsreen = false;
  GEngine.windowApi.bShouldClose = false;
  SWindow.window = hWin;
  SWindow.device = SApiUser32.GetDC(hWin);
  GT_LOG(LOG_INFO, "API:Win32 Create Window => Width:%d Height:%d Title:%s", Width, Height, Title);
}

Void ApiWin32WindowUpdate() {
  MSG msg;
  memcpy(GEngine.inputApi.previousKeys, GEngine.inputApi.currentKeys, sizeof(GEngine.inputApi.previousKeys));
  InternalUpdateKey(MOUSE_FORWARD_CODE, false);
  InternalUpdateKey(MOUSE_BACKWARD_CODE, false);

  while (SApiUser32.PeekMessageA(&msg, SWindow.window, 0, 0, PM_REMOVE)) {
    if (msg.message == CODE_WINDOW_CLOSE) {
      GEngine.windowApi.bShouldClose = true;
      return;
    }
    SApiUser32.TranslateMessage(&msg);
    SApiUser32.DispatchMessageA(&msg);
  }
}

Void ApiWin32WindowDestroy() {
  GT_LOG(LOG_INFO, "API:Win32 Close Window");
  SApiUser32.DestroyWindow(SWindow.window);
}

static LRESULT InternalWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  switch (Msg) {
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
      Int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
      UInt32 key = (delta > 0) ? MOUSE_FORWARD_CODE : MOUSE_BACKWARD_CODE;
      InternalUpdateKey(key, true);
    } break;
    default: {
      return SApiUser32.DefWindowProcA(hWnd, Msg, wParam, lParam);
    }
  }

  return 0;
}

static Void InternalUpdateKey(UInt64 KeyCode, Bool bIsPressed) {
  static UInt32 vkCode[] = {
      VK_OEM_7,           // Key: '
      VK_OEM_COMMA,       // Key: ,
      VK_OEM_MINUS,       // Key: -
      VK_OEM_PERIOD,      // Key: .
      VK_OEM_2,           // Key: /
      0x30,               // Key: 0
      0x31,               // Key: 1
      0x32,               // Key: 2
      0x33,               // Key: 3
      0x34,               // Key: 4
      0x35,               // Key: 5
      0x36,               // Key: 6
      0x37,               // Key: 7
      0x38,               // Key: 8
      0x39,               // Key: 9
      VK_OEM_1,           // Key: ;
      VK_OEM_NEC_EQUAL,   // Key: =
      0x41,               // Key: A
      0x42,               // Key: B
      0x43,               // Key: C
      0x44,               // Key: D
      0x45,               // Key: E
      0x46,               // Key: F
      0x47,               // Key: G
      0x48,               // Key: H
      0x49,               // Key: I
      0x4A,               // Key: J
      0x4B,               // Key: K
      0x4C,               // Key: L
      0x4D,               // Key: M
      0x4E,               // Key: N
      0x4F,               // Key: O
      0x50,               // Key: P
      0x51,               // Key: Q
      0x52,               // Key: R
      0x53,               // Key: S
      0x54,               // Key: T
      0x55,               // Key: U
      0x56,               // Key: V
      0x57,               // Key: W
      0x58,               // Key: X
      0x59,               // Key: Y
      0x5A,               // Key: Z
      VK_OEM_4,           // Key: [{
      VK_OEM_102,         // Key: '\'
      VK_OEM_6,           // Key: ]}
      VK_OEM_3,           // Key: `
      VK_SPACE,           // Key: Space
      VK_ESCAPE,          // Key: Escape
      VK_RETURN,          // Key: Enter
      VK_TAB,             // Key: Tab
      VK_BACK,            // Key: Backspace
      VK_INSERT,          // Key: Insert
      VK_DELETE,          // Key: Delete
      VK_RIGHT,           // Key: Right
      VK_LEFT,            // Key: Left
      VK_DOWN,            // Key: Down
      VK_UP,              // Key: Up
      VK_NEXT,            // Key: Page Down
      VK_PRIOR,           // Key: Page Up
      VK_HOME,            // Key: Home
      VK_END,             // Key: End
      VK_CAPITAL,         // Key: Caps Look
      VK_SCROLL,          // Key: Scroll Down
      VK_NUMLOCK,         // Key: Num Lock
      VK_PRINT,           // Key: Print
      VK_PAUSE,           // Key: Pause
      VK_F1,              // Key: F1
      VK_F2,              // Key: F2
      VK_F3,              // Key: F3
      VK_F4,              // Key: F4
      VK_F5,              // Key: F5
      VK_F6,              // Key: F6
      VK_F7,              // Key: F7
      VK_F8,              // Key: F8
      VK_F9,              // Key: F9
      VK_F10,             // Key: F10
      VK_F11,             // Key: F11
      VK_F12,             // Key: F12
      VK_SHIFT,           // Key: Left Shift
      VK_CONTROL,         // Key: Left Control
      VK_MENU,            // Key: Left Alt
      VK_LWIN,            // Key: Left Super
      VK_RSHIFT,          // Key: Left Shift
      VK_RCONTROL,        // Key: Right Control
      VK_RMENU,           // Key: Right Alt
      VK_RWIN,            // Key: Right Super
      VK_MENU,            // Key: Menu
      0x30,               // Key: 0
      0x31,               // Key: 1
      0x32,               // Key: 2
      0x33,               // Key: 3
      0x34,               // Key: 4
      0x35,               // Key: 5
      0x36,               // Key: 6
      0x37,               // Key: 7
      0x38,               // Key: 8
      0x39,               // Key: 9
      VK_DECIMAL,         // Key: de decimal
      VK_DIVIDE,          // Key: de divisão
      VK_MULTIPLY,        // Key: de multiplicação
      VK_SUBTRACT,        // Key: de subtração
      VK_ADD,             // Key: de adição
      VK_RETURN,          // Key: Enter
      VK_OEM_NEC_EQUAL,   // Key: Equal
      MOUSE_LEFT_CODE,    // Mouse Left
      MOUSE_MIDDLE_CODE,  // Mouse Middle
      MOUSE_RIGHT_CODE,   // Mouse Right
      MOUSE_FORWARD_CODE, // Mouse Forward
      MOUSE_BACKWARD_CODE // Mouse Backward
  };

  for (Int32 c = 0; c < KEY_MAX; c++) {
    if (vkCode[c] == KeyCode) {
      GEngine.inputApi.currentKeys[c] = bIsPressed;
      break;
    }
  }
}

Bool ApiWin32Init(FWindowApi* WindowApi) {
  SLibUser32 = EngineLoadModule("user32.dll");
  if (SLibUser32 == NULL) {
    return false;
  }
  EngineLoadApi(SLibUser32, &SApiUser32, SLibUser32Names, false);
  WindowApi->OnWindowCreate = &ApiWin32WindowCreate;
  WindowApi->OnWindowUpdate = &ApiWin32WindowUpdate;
  WindowApi->OnWindowDestroy = &ApiWin32WindowDestroy;
  return true;
}

#endif // PLATFORM_WINDOWS

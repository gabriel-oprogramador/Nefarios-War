#pragma once
/*Game Template or GT-Engine is my study-focused engine written in C made from scratch to create simple games.*/
/*Original Project by Gabriel Oliveira Prudente @gabriel_Oprogramador*/

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_EXPAND(a)                           #a
#define STR(a)                                 NO_EXPAND(a)
#define FIND_CONFIG(ConfigPath)                STR(CONFIG_PATH) ConfigPath
#define FIND_ASSET(AssetPath)                  STR(CONTENT_PATH) AssetPath
#define CONTEXT_LOG                            "-> " STR(__FILE__) ":" STR(__LINE__)
#define MAKE_LOG_FAST(LogLevel)                ((UInt16)1 << 8 | LogLevel)
#define BUFFER_SMALL                           256
#define BUFFER_LOG_SIZE                        2048
#define GT_LOG(ELogLevel, Format, ...)         EnginePrintLog(ELogLevel, CONTEXT_LOG, Format, ##__VA_ARGS__)
#define GT_LOGTEMP(ELogLevel, Format, ...)     EnginePrintLog(MAKE_LOG_FAST(ELogLevel), CONTEXT_LOG, Format, ##__VA_ARGS__)
#define CALL_API(Function, DefaultReturn, ...) (Function != NULL) ? Function(__VA_ARGS__) : DefaultReturn;
#define FCOLOR_GL(FCOLOR)                      FCOLOR.r, FCOLOR.g, FCOLOR.b, FCOLOR.a

#ifdef _MSC_VER
#define GT_INLINE static __forceinline
#else
#define GT_INLINE static inline
#endif  // _MSC_VER

typedef void Void;
typedef bool Bool;
typedef char Char;
typedef short Int16;
typedef int Int32;
typedef long long Int64;
typedef unsigned char UChar;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;
typedef float Float;
typedef double Double;
typedef const char* String;

typedef struct {
  Float r;
  Float g;
  Float b;
  Float a;
} FColor;

typedef struct {
  Float x;
  Float y;
} FVector2;

typedef struct {
  Float x;
  Float y;
  Float z;
} FVector;

typedef struct {
  Float pitch;
  Float yaw;
  Float roll;
} FRotator;

typedef enum {
  IM_GAME_ONLY,
  IM_UI_ONLY,
  IM_GAME_UI,
  IM_EDITOR_ONLY
} EInputMode;

typedef enum {
  LOG_INFO = (1 << 0),
  LOG_SUCCESS = (1 << 1),
  LOG_WARNING = (1 << 2),
  LOG_ERROR = (1 << 3),
  LOG_FATAL = (1 << 4),
} ELogLevel;

typedef enum {
  KEY_APOSTROPHE,            // Key: '
  KEY_COMMA,                 // Key: ,
  KEY_MINUS,                 // Key: -
  KEY_PERIOD,                // Key: .
  KEY_SLASH,                 // Key: /
  KEY_ZERO,                  // Key: 0
  KEY_ONE,                   // Key: 1
  KEY_TWO,                   // Key: 2
  KEY_THREE,                 // Key: 3
  KEY_FOUR,                  // Key: 4
  KEY_FIVE,                  // Key: 5
  KEY_SIX,                   // Key: 6
  KEY_SEVEN,                 // Key: 7
  KEY_EIGHT,                 // Key: 8
  KEY_NINE,                  // Key: 9
  KEY_SEMICOLON,             // Key: ;
  KEY_EQUAL,                 // Key: =
  KEY_A,                     // Key: A | a
  KEY_B,                     // Key: B | b
  KEY_C,                     // Key: C | c
  KEY_D,                     // Key: D | d
  KEY_E,                     // Key: E | e
  KEY_F,                     // Key: F | f
  KEY_G,                     // Key: G | g
  KEY_H,                     // Key: H | h
  KEY_I,                     // Key: I | i
  KEY_J,                     // Key: J | j
  KEY_K,                     // Key: K | k
  KEY_L,                     // Key: L | l
  KEY_M,                     // Key: M | m
  KEY_N,                     // Key: N | n
  KEY_O,                     // Key: O | o
  KEY_P,                     // Key: P | p
  KEY_Q,                     // Key: Q | q
  KEY_R,                     // Key: R | r
  KEY_S,                     // Key: S | s
  KEY_T,                     // Key: T | t
  KEY_U,                     // Key: U | u
  KEY_V,                     // Key: V | v
  KEY_W,                     // Key: W | w
  KEY_X,                     // Key: X | x
  KEY_Y,                     // Key: Y | y
  KEY_Z,                     // Key: Z | z
  KEY_LEFT_BRACKET,          // Key: [
  KEY_BACKSLASH,             // Key: '\'
  KEY_RIGHT_BRACKET,         // Key: ]
  KEY_GRAVE,                 // Key: `
  KEY_SPACE,                 // Key: Space
  KEY_ESCAPE,                // Key: Esc
  KEY_ENTER,                 // Key: Enter
  KEY_TAB,                   // Key: Tab
  KEY_BACKSPACE,             // Key: Backspace
  KEY_INSERT,                // Key: Ins
  KEY_DELETE,                // Key: Del
  KEY_RIGHT,                 // Key: Cursor right
  KEY_LEFT,                  // Key: Cursor left
  KEY_DOWN,                  // Key: Cursor down
  KEY_UP,                    // Key: Cursor up
  KEY_PAGE_UP,               // Key: Page up
  KEY_PAGE_DOWN,             // Key: Page down
  KEY_HOME,                  // Key: Home
  KEY_END,                   // Key: End
  KEY_CAPS_LOCK,             // Key: Caps lock
  KEY_SCROLL_LOCK,           // Key: Scroll down
  KEY_NUM_LOCK,              // Key: Num lock
  KEY_PRINT_SCREEN,          // Key: Print screen
  KEY_PAUSE,                 // Key: Pause
  KEY_F1,                    // Key: F1
  KEY_F2,                    // Key: F2
  KEY_F3,                    // Key: F3
  KEY_F4,                    // Key: F4
  KEY_F5,                    // Key: F5
  KEY_F6,                    // Key: F6
  KEY_F7,                    // Key: F7
  KEY_F8,                    // Key: F8
  KEY_F9,                    // Key: F9
  KEY_F10,                   // Key: F10
  KEY_F11,                   // Key: F11
  KEY_F12,                   // Key: F12
  KEY_LEFT_SHIFT,            // Key: Shift left
  KEY_LEFT_CONTROL,          // Key: Control left
  KEY_LEFT_ALT,              // Key: Alt left
  KEY_LEFT_SUPER,            // Key: Super left
  KEY_RIGHT_SHIFT,           // Key: Shift right
  KEY_RIGHT_CONTROL,         // Key: Control right
  KEY_RIGHT_ALT,             // Key: Alt right
  KEY_RIGHT_SUPER,           // Key: Super right
  KEY_KB_MENU,               // Key: KB menu
  KEY_KP_0,                  // Key: Keypad 0
  KEY_KP_1,                  // Key: Keypad 1
  KEY_KP_2,                  // Key: Keypad 2
  KEY_KP_3,                  // Key: Keypad 3
  KEY_KP_4,                  // Key: Keypad 4
  KEY_KP_5,                  // Key: Keypad 5
  KEY_KP_6,                  // Key: Keypad 6
  KEY_KP_7,                  // Key: Keypad 7
  KEY_KP_8,                  // Key: Keypad 8
  KEY_KP_9,                  // Key: Keypad 9
  KEY_KP_DECIMAL,            // Key: Keypad .
  KEY_KP_DIVIDE,             // Key: Keypad /
  KEY_KP_MULTIPLY,           // Key: Keypad *
  KEY_KP_SUBTRACT,           // Key: Keypad -
  KEY_KP_ADD,                // Key: Keypad +
  KEY_KP_ENTER,              // Key: Keypad Enter
  KEY_KP_EQUAL,              // Key: Keypad =
  KEY_MOUSE_LEFT,            // Mouse Left
  KEY_MOUSE_MIDDLE,          // Mouse Middle
  KEY_MOUSE_RIGHT,           // Mouse Right
  KEY_MOUSE_WHEEL_FORWARD,   // Mouse Forward
  KEY_MOUSE_WHEEL_BACKWARD,  // Mouse Backward
  KEY_MAX
} EKeyCode;

// clang-format off
// Red
#define COLOR_RED            (FColor){ 1.0f, 0.0f, 0.0f, 1.0f } // RGB: 255, 0, 0
#define COLOR_RED_LIGHT      (FColor){ 1.0f, 0.5f, 0.5f, 1.0f } // RGB: 255, 128, 128
#define COLOR_RED_DARK       (FColor){ 0.5f, 0.0f, 0.0f, 1.0f } // RGB: 128, 0, 0

// Green
#define COLOR_GREEN          (FColor){ 0.0f, 1.0f, 0.0f, 1.0f } // RGB: 0, 255, 0
#define COLOR_GREEN_LIGH     (FColor){ 0.5f, 1.0f, 0.5f, 1.0f } // RGB: 128, 255, 128
#define COLOR_GREEN_DARK     (FColor){ 0.0f, 0.5f, 0.0f, 1.0f } // RGB: 0, 128, 0

// Blue
#define COLOR_BLUE           (FColor){ 0.0f, 0.0f, 1.0f, 1.0f } // RGB: 0, 0, 255
#define COLOR_BLUE_LIGHT     (FColor){ 0.5f, 0.5f, 1.0f, 1.0f } // RGB: 128, 128, 255
#define COLOR_BLUE_DARK      (FColor){ 0.0f, 0.0f, 0.5f, 1.0f } // RGB: 0, 0, 128

// Yellow
#define COLOR_YELLOW         (FColor){ 1.0f, 1.0f, 0.0f, 1.0f } // RGB: 255, 255, 0
#define COLOR_YELLOW_LIGHT   (FColor){ 1.0f, 1.0f, 0.5f, 1.0f } // RGB: 255, 255, 128
#define COLOR_YELLOW_DARK     (FColor){ 0.5f, 0.5f, 0.0f, 1.0f } // RGB: 128, 128, 0

// Cyan
#define COLOR_CYAN           (FColor){ 0.0f, 1.0f, 1.0f, 1.0f } // RGB: 0, 255, 255
#define COLOR_CYAN_LIGHT     (FColor){ 0.5f, 1.0f, 1.0f, 1.0f } // RGB: 128, 255, 255
#define COLOR_CYAN_DARK      (FColor){ 0.0f, 0.5f, 0.5f, 1.0f } // RGB: 0, 128, 128

// Magenta
#define COLOR_MAGENTA        (FColor){ 1.0f, 0.0f, 1.0f, 1.0f } // RGB: 255, 0, 255
#define COLOR_MAGENTA_LIGHT  (FColor){ 1.0f, 0.5f, 1.0f, 1.0f } // RGB: 255, 128, 255
#define COLOR_MAGENTA_DARK   (FColor){ 0.5f, 0.0f, 0.5f, 1.0f } // RGB: 128, 0, 128

// Orange
#define COLOR_ORANGE         (FColor){ 1.0f, 0.65f, 0.0f, 1.0f } // RGB: 255, 165, 0
#define COLOR_ORANGE_LIGHT   (FColor){ 1.0f, 0.8f, 0.5f, 1.0f } // RGB: 255, 204, 128
#define COLOR_ORANGE_DARK    (FColor){ 0.5f, 0.32f, 0.0f, 1.0f } // RGB: 128, 82, 0

// Purple
#define COLOR_PURPLE         (FColor){ 0.5f, 0.0f, 0.5f, 1.0f } // RGB: 128, 0, 128
#define COLOR_PURPLE_LIGHT   (FColor){ 0.75f, 0.5f, 0.75f, 1.0f } // RGB: 192, 128, 192
#define COLOR_PURPLE_DARK    (FColor){ 0.25f, 0.0f, 0.25f, 1.0f } // RGB: 64, 0, 64

// Brown
#define COLOR_BROWN          (FColor){ 0.65f, 0.16f, 0.16f, 1.0f } // RGB: 165, 42, 42
#define COLOR_BROWN_LIGHT    (FColor){ 0.82f, 0.41f, 0.41f, 1.0f } // RGB: 210, 105, 105
#define COLOR_BROWN_DARK     (FColor){ 0.33f, 0.08f, 0.08f, 1.0f } // RGB: 84, 21, 21

// Pink
#define COLOR_PINK           (FColor){ 1.0f, 0.75f, 0.8f, 1.0f } // RGB: 255, 192, 203
#define COLOR_PINK_LIGHT     (FColor){ 1.0f, 0.88f, 0.88f, 1.0f } // RGB: 255, 224, 224
#define COLOR_PINK_DARK      (FColor){ 0.5f, 0.38f, 0.4f, 1.0f } // RGB: 128, 96, 102

// Gold
#define COLOR_GOLD           (FColor){ 1.0f, 0.84f, 0.0f, 1.0f } // RGB: 255, 215, 0
#define COLOR_GOLD_LIGHT     (FColor){ 1.0f, 0.92f, 0.5f, 1.0f } // RGB: 255, 234, 128
#define COLOR_GOLD_DARK      (FColor){ 0.5f, 0.42f, 0.0f, 1.0f } // RGB: 128, 107, 0

// Silver
#define COLOR_SILVER         (FColor){ 0.75f, 0.75f, 0.75f, 1.0f } // RGB: 192, 192, 192
#define COLOR_SILVER_LIGHT   (FColor){ 0.88f, 0.88f, 0.88f, 1.0f } // RGB: 224, 224, 224
#define COLOR_SILVER_DARK    (FColor){ 0.38f, 0.38f, 0.38f, 1.0f } // RGB: 96, 96, 96

// Lime
#define COLOR_LIME           (FColor){ 0.0f, 1.0f, 0.0f, 1.0f } // RGB: 0, 255, 0
#define COLOR_LIME_LIGHT     (FColor){ 0.5f, 1.0f, 0.5f, 1.0f } // RGB: 128, 255, 128
#define COLOR_LIME_DARK      (FColor){ 0.0f, 0.5f, 0.0f, 1.0f } // RGB: 0, 128, 0

// Gray
#define COLOR_GRAY           (FColor){ 0.5f, 0.5f, 0.5f, 1.0f }
#define COLOR_GRAY_LIGHT     (FColor){ 0.7f, 0.7f, 0.7f, 1.0f }
#define COLOR_GRAY_DARK      (FColor){ 0.3f, 0.3f, 0.3f, 1.0f }

// Sky Blue
#define COLOR_SKY_BLUE       (FColor){ 0.53f, 0.81f, 0.92f, 1.0f }
#define COLOR_SKY_BLUE_LIGHT (FColor){ 0.73f, 0.91f, 1.0f, 1.0f }
#define COLOR_SKY_BLUE_DARK  (FColor){ 0.3f, 0.6f, 0.8f, 1.0f  }

// White
#define COLOR_WHITE          (FColor){ 1.0f, 1.0f, 1.0f, 1.0f }
#define COLOR_WHITE_LIGHT    (FColor){ 1.0f, 1.0f, 1.0f, 1.0f }
#define COLOR_WHITE_DARK     (FColor){ 0.8f, 0.8f, 0.8f, 1.0f }

// Chrominum
#define COLOR_CHROMIUM       (FColor){ 0.8f, 0.8f, 0.8f, 1.0f }
#define COLOR_CHROMIUM_LIGHT (FColor){ 0.9f, 0.9f, 0.9f, 1.0f }
#define COLOR_CHROMIUM_DARK  (FColor){ 0.6f, 0.6f, 0.6f, 1.0f }

// Copper
#define COLOR_COPPER         (FColor){ 0.72f, 0.45f, 0.2f, 1.0f }
#define COLOR_COPPER_LIGHT   (FColor){ 0.85f, 0.55f, 0.25f, 1.0f }
#define COLOR_COPPER_DARK    (FColor){ 0.6f, 0.35f, 0.15f, 1.0f }

// Iren
#define COLOR_IRON           (FColor){ 0.5f, 0.5f, 0.5f, 1.0f }
#define COLOR_IRON_LIGHT     (FColor){ 0.7f, 0.7f, 0.7f, 1.0f }
#define COLOR_IRON_DARK      (FColor){ 0.3f, 0.3f, 0.3f, 1.0f }

// Black
#define COLOR_BLACK          (FColor){ 0.0f, 0.0f, 0.0f, 1.0f }
#define COLOR_BLACK_LIGHT    (FColor){ 0.2f, 0.2f, 0.2f, 1.0f }
#define COLOR_BLACK_DARK     (FColor){ 0.0f, 0.0f, 0.0f, 1.0f }

// BLANK
#define COLOR_BLANK          (FColor){ 0.0f, 0.0f, 0.0f, 0.0f }
// clang-format on

extern Void EnginePrintLog(ELogLevel Level, String Context, String Format, ...);

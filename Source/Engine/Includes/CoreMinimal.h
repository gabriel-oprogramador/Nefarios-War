#pragma once
/*Game Template or GT-Engine is my study-focused engine written in C made from scratch to create simple games.*/
/*Original Project by Gabriel Oliveira Prudente @gabriel_Oprogramador*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_EXPAND(a)                       #a
#define STR(a)                             NO_EXPAND(a)
#define FIND_CONFIG(ConfigPath)            STR(CONFIG_PATH) ConfigPath
#define FIND_ASSET(AssetPath)              STR(CONTENT_PATH) AssetPath
#define CONTEXT_LOG                        "-> " STR(__FILE__) ":" STR(__LINE__)
#define MAKE_LOG_FAST(LogLevel)            ((UInt16)1 << 8 | LogLevel)
#define BUFFER_SMALL                        256
#define BUFFER_LOG_SIZE                    2048
#define GT_LOG(ELogLevel, Format, ...)     EnginePrintLog(ELogLevel, CONTEXT_LOG, Format, ##__VA_ARGS__)
#define GT_LOGTEMP(ELogLevel, Format, ...) EnginePrintLog(MAKE_LOG_FAST(ELogLevel), CONTEXT_LOG, Format, ##__VA_ARGS__)

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

typedef enum {
  LOG_INFO = (1 << 0),    //
  LOG_SUCCESS = (1 << 1), //
  LOG_WARNING = (1 << 2), //
  LOG_ERROR = (1 << 3),   //
  LOG_FATAL = (1 << 4),   //
} ELogLevel;

typedef enum {
  KEY_APOSTROPHE,           // Key: '
  KEY_COMMA,                // Key: ,
  KEY_MINUS,                // Key: -
  KEY_PERIOD,               // Key: .w
  KEY_SLASH,                // Key: /
  KEY_ZERO,                 // Key: 0
  KEY_ONE,                  // Key: 1
  KEY_TWO,                  // Key: 2
  KEY_THREE,                // Key: 3
  KEY_FOUR,                 // Key: 4
  KEY_FIVE,                 // Key: 5
  KEY_SIX,                  // Key: 6
  KEY_SEVEN,                // Key: 7
  KEY_EIGHT,                // Key: 8
  KEY_NINE,                 // Key: 9
  KEY_SEMICOLON,            // Key: ;
  KEY_EQUAL,                // Key: =
  KEY_A,                    // Key: A | a
  KEY_B,                    // Key: B | b
  KEY_C,                    // Key: C | c
  KEY_D,                    // Key: D | d
  KEY_E,                    // Key: E | e
  KEY_F,                    // Key: F | f
  KEY_G,                    // Key: G | g
  KEY_H,                    // Key: H | h
  KEY_I,                    // Key: I | i
  KEY_J,                    // Key: J | j
  KEY_K,                    // Key: K | k
  KEY_L,                    // Key: L | l
  KEY_M,                    // Key: M | m
  KEY_N,                    // Key: N | n
  KEY_O,                    // Key: O | o
  KEY_P,                    // Key: P | p
  KEY_Q,                    // Key: Q | q
  KEY_R,                    // Key: R | r
  KEY_S,                    // Key: S | s
  KEY_T,                    // Key: T | t
  KEY_U,                    // Key: U | u
  KEY_V,                    // Key: V | v
  KEY_W,                    // Key: W | w
  KEY_X,                    // Key: X | x
  KEY_Y,                    // Key: Y | y
  KEY_Z,                    // Key: Z | z
  KEY_LEFT_BRACKET,         // Key: [
  KEY_BACKSLASH,            // Key: '\'
  KEY_RIGHT_BRACKET,        // Key: ]
  KEY_GRAVE,                // Key: `
  KEY_SPACE,                // Key: Space
  KEY_ESCAPE,               // Key: Esc
  KEY_ENTER,                // Key: Enter
  KEY_TAB,                  // Key: Tab
  KEY_BACKSPACE,            // Key: Backspace
  KEY_INSERT,               // Key: Ins
  KEY_DELETE,               // Key: Del
  KEY_RIGHT,                // Key: Cursor right
  KEY_LEFT,                 // Key: Cursor left
  KEY_DOWN,                 // Key: Cursor down
  KEY_UP,                   // Key: Cursor up
  KEY_PAGE_UP,              // Key: Page up
  KEY_PAGE_DOWN,            // Key: Page down
  KEY_HOME,                 // Key: Home
  KEY_END,                  // Key: End
  KEY_CAPS_LOCK,            // Key: Caps lock
  KEY_SCROLL_LOCK,          // Key: Scroll down
  KEY_NUM_LOCK,             // Key: Num lock
  KEY_PRINT_SCREEN,         // Key: Print screen
  KEY_PAUSE,                // Key: Pause
  KEY_F1,                   // Key: F1
  KEY_F2,                   // Key: F2
  KEY_F3,                   // Key: F3
  KEY_F4,                   // Key: F4
  KEY_F5,                   // Key: F5
  KEY_F6,                   // Key: F6
  KEY_F7,                   // Key: F7
  KEY_F8,                   // Key: F8
  KEY_F9,                   // Key: F9
  KEY_F10,                  // Key: F10
  KEY_F11,                  // Key: F11
  KEY_F12,                  // Key: F12
  KEY_LEFT_SHIFT,           // Key: Shift left
  KEY_LEFT_CONTROL,         // Key: Control left
  KEY_LEFT_ALT,             // Key: Alt left
  KEY_LEFT_SUPER,           // Key: Super left
  KEY_RIGHT_SHIFT,          // Key: Shift right
  KEY_RIGHT_CONTROL,        // Key: Control right
  KEY_RIGHT_ALT,            // Key: Alt right
  KEY_RIGHT_SUPER,          // Key: Super right
  KEY_KB_MENU,              // Key: KB menu
  KEY_KP_0,                 // Key: Keypad 0
  KEY_KP_1,                 // Key: Keypad 1
  KEY_KP_2,                 // Key: Keypad 2
  KEY_KP_3,                 // Key: Keypad 3
  KEY_KP_4,                 // Key: Keypad 4
  KEY_KP_5,                 // Key: Keypad 5
  KEY_KP_6,                 // Key: Keypad 6
  KEY_KP_7,                 // Key: Keypad 7
  KEY_KP_8,                 // Key: Keypad 8
  KEY_KP_9,                 // Key: Keypad 9
  KEY_KP_DECIMAL,           // Key: Keypad .
  KEY_KP_DIVIDE,            // Key: Keypad /
  KEY_KP_MULTIPLY,          // Key: Keypad *
  KEY_KP_SUBTRACT,          // Key: Keypad -
  KEY_KP_ADD,               // Key: Keypad +
  KEY_KP_ENTER,             // Key: Keypad Enter
  KEY_KP_EQUAL,             // Key: Keypad =
  KEY_MOUSE_LEFT,           // Mouse Left
  KEY_MOUSE_MIDDLE,         // Mouse Middle
  KEY_MOUSE_RIGHT,          // Mouse Right
  KEY_MOUSE_WHEEL_FORWARD,  // Mouse Forward
  KEY_MOUSE_WHEEL_BACKWARD, // Mouse Backward
  KEY_MAX
} EKeyCode;

extern Void EnginePrintLog(ELogLevel Level, String Context, String Format, ...);

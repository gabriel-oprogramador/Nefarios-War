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
#define GT_LOG(ELogLevel, Format, ...)     EnginePrintLog(ELogLevel, "GAME:", CONTEXT_LOG, Format, ##__VA_ARGS__)
#define GT_LOGTEMP(ELogLevel, Format, ...) EnginePrintLog(MAKE_LOG_FAST(ELogLevel), "GAME:", CONTEXT_LOG, Format, ##__VA_ARGS__)

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

extern Void EnginePrintLog(ELogLevel Level, String Module, String Context, String Format, ...);

#ifdef PLATFORM_WINDOWS
#include "Engine.h"

#include <windows.h>

static FILE* SLogFile = NULL;
static String SLogFilePath = "LogFile.txt";

static struct {
  HANDLE hConsole;
  Int32 defaultAttribute;
} SConsole;

static Void InitWin32Console() {
  SConsole.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenInfo;
  GetConsoleScreenBufferInfo(SConsole.hConsole, &consoleScreenInfo);
  SConsole.defaultAttribute = consoleScreenInfo.wAttributes;
}

Void EngineInit(Int32 Width, Int32 Height, String Title) {
  InitWin32Console();
  SLogFile = fopen(SLogFilePath, "w");
}

Bool EngineShouldClose() {
  return false;
}

Void EngineShutdown() {
}

Void EnginePrintLog(ELogLevel Level, String Module, String Context, String Format, ...) {
  static Char logBuffer[2048] = {""};
  static String logTag = NULL;
  Bool bIsFast = (Level >> 8);
  UInt16 logLevel = Level & 0xFF;

  enum {
    LC_WHITE = 15,   //
    LC_GREEN = 10,   //
    LC_YELLOW = 14,  //
    LC_RED = 12,     //
    LC_DARK_RED = 4, //
  } color;

  switch (logLevel) {
    case LOG_INFO: {
      logTag = (String) "[LOG INFO] =>";
      color = LC_WHITE;
    } break;
    case LOG_SUCCESS: {
      logTag = (String) "[LOG SUCCESS] =>";
      color = LC_GREEN;
    } break;
    case LOG_WARNING: {
      logTag = (String) "[LOG WARNING] =>";
      color = LC_YELLOW;
    } break;
    case LOG_ERROR: {
      logTag = (String) "[LOG ERROR] =>";
      color = LC_RED;
    } break;
    case LOG_FATAL: {
      logTag = (String) "[LOG FATAL] =>";
      color = LC_DARK_RED;
    } break;
  }
  SetConsoleTextAttribute(SConsole.hConsole, color);
  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s %s %s %s\n", logTag, Module, Format, (Level == LOG_INFO) ? "" : Context);
  vprintf(logBuffer, args);
  va_end(args);
  SetConsoleTextAttribute(SConsole.hConsole, SConsole.defaultAttribute);

  if (SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif // PLATFORM_WINDOWS

#ifdef PLATFORM_LINUX
#include "CoreMinimal.h"

static FILE* SLogFile = NULL;
static String SLogFilePath = "LogFile.txt";

Void EngineInit(Int32 Width, Int32 Height, String Title) {
  SLogFile = fopen(SLogFilePath, "w");
}

Bool EngineShouldClose() {
  return true;
}

Void EngineShutdown() {
}

Void EngineBeginFrame() {
}

Void EngineEndFrame() {
}

Void EnginePrintLog(ELogLevel Level, String Module, String Context, String Format, ...) {
  static Char logBuffer[BUFFER_LOG_SIZE] = {""};
  static String logTag = NULL;
  static String logColor = NULL;
  Bool bIsFast = (Level >> 8);
  UInt16 logLevel = Level & 0xFF;

  switch (logLevel) {
    case LOG_INFO: {
      logColor = (String)"\033[0;97m";
      logTag = (String) "[LOG INFO] =>";
    } break;
    case LOG_SUCCESS: {
      logColor = (String)"\033[0;92m";
      logTag = (String) "[LOG SUCCESS] =>";
    } break;
    case LOG_WARNING: {
      logColor = (String)"\033[0;93m";
      logTag = (String) "[LOG WARNING] =>";
    } break;
    case LOG_ERROR: {
      logColor = (String)"\033[0;91m";
      logTag = (String) "[LOG ERROR] =>";
    } break;
    case LOG_FATAL: {
      logColor = (String)"\033[0;31m";
      logTag = (String) "[LOG FATAL] =>";
    } break;
  }

  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s %s %s %s %s\033[0m\n", logColor, logTag, Module, Format, (logLevel == LOG_INFO) ? "" : Context);
  vprintf(logBuffer, args);
  va_end(args);

  if (SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    snprintf(logBuffer, sizeof(logBuffer), "%s %s %s %s\n", logTag, Module, Format, (logLevel == LOG_INFO) ? "" : Context);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif // PLATFORM_LINUX

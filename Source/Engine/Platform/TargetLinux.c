#ifdef PLATFORM_LINUX
#include "Engine.h"

#include <dlfcn.h>
#include <sys/time.h>
#include <time.h>

#define TO_SECONDS 1000000.0

FGT GEngine = {0};

extern Bool ApiX11Init(IWindowApi* WindowApi);
extern Void ApiEGLSwapBuffer();

static FILE* SLogFile = NULL;
static String SLogFilePath = "LogFile.txt";
static Double SFrameStartTime = 0;

Void EngineInit(Int32 Width, Int32 Height, String Title) {
  GEngine.timerApi.engineStartTime = EngineGetTime();
  SLogFile = fopen(SLogFilePath, "w");
  if (ApiX11Init(&GEngine.windowApi)) {
    CALL_API(GEngine.windowApi.OnWindowCreate, NULL, Width, Height, Title);
  }
}

Bool EngineShouldClose() {
  return GEngine.windowApi.bShouldClose;
}

Void EngineShutdown() {
  CALL_API(GEngine.windowApi.OnWindowDestroy, NULL);
}

Void EngineBeginFrame() {
  SFrameStartTime = EngineGetTime();
  CALL_API(GEngine.windowApi.OnWindowUpdate, NULL);
}

Void EngineEndFrame() {
  ApiEGLSwapBuffer();

  Double end = EngineGetTime();
  Double delta = end - SFrameStartTime;
  Double target = GEngine.timerApi.frameTime;
  Double remainingTime = target - delta;

  if (target > 0) {
    if (remainingTime > (delta * 0.9)) {
      EngineWait(remainingTime);
    }
    while ((delta = EngineGetTime() - SFrameStartTime) < target) {
    }
  }
  GEngine.timerApi.deltaTime = delta;
  GEngine.timerApi.frameRate = ceil(1.0f / delta);
}

Void EngineFullscreen(Bool bFullscreen) {
  CALL_API(GEngine.windowApi.OnWindowFullscreen, NULL, bFullscreen);
}

Double EngineGetTime() {
  struct timespec timeSpec;
  clock_gettime(CLOCK_MONOTONIC, &timeSpec);
  return (Double)timeSpec.tv_sec + (Double)timeSpec.tv_nsec / 1000000000.0f;
}

Void EngineSetTargetFPS(UInt32 Target) {
  GEngine.timerApi.frameTime = 1.f / Target;
}

Void EngineWait(Double Milliseconds) {
  struct timespec time;
  Int32 sec = (Int32)(Milliseconds / 1000);
  Int32 nSeg = (Int32)(Milliseconds - sec * 1000) * 1000000.0f;

  time.tv_sec = sec;
  time.tv_nsec = nSeg;
  nanosleep(&time, NULL);
}

// Module
Void* EngineLoadModule(String Name) {
  return dlopen(Name, RTLD_LAZY | RTLD_GLOBAL);
}

Void EngineFreeModule(Void* Module) {
  if (Module != NULL) {
    dlclose(Module);
  }
}

Void* EngineGetFunc(Void* Module, String Name) {
  return dlsym(Module, Name);
}

Void EngineLoadApi(Void* Module, Void* Api, String* Names, Bool bDebugMode) {
  String* names = Names;
  Void* function = NULL;
  Int32 index = 0;

  while (**names) {
    function = EngineGetFunc(Module, *names);
    if (function != NULL) {
      Void* addr = (Char*)Api + index * sizeof(Void*);
      memcpy(addr, &function, sizeof(Void*));
      if (bDebugMode) {
        GT_LOG(LOG_INFO, "Function Loaded:%s", *names);
      }
    } else if (bDebugMode) {
      GT_LOG(LOG_INFO, "Function Not Loaded:%s", *names);
    }
    names++;
    index++;
  }
}

Void EnginePrintLog(ELogLevel Level, String Context, String Format, ...) {
  static Char logBuffer[BUFFER_LOG_SIZE] = {""};
  static String logTag = NULL;
  static String logColor = NULL;
  Bool bIsFast = (Level >> 8);
  UInt16 logLevel = Level & 0xFF;

  switch (logLevel) {
    case LOG_INFO: {
      logColor = (String) "\033[0;97m";
      logTag = (String) "[LOG INFO] =>";
    } break;
    case LOG_SUCCESS: {
      logColor = (String) "\033[0;92m";
      logTag = (String) "[LOG SUCCESS] =>";
    } break;
    case LOG_WARNING: {
      logColor = (String) "\033[0;93m";
      logTag = (String) "[LOG WARNING] =>";
    } break;
    case LOG_ERROR: {
      logColor = (String) "\033[0;91m";
      logTag = (String) "[LOG ERROR] =>";
    } break;
    case LOG_FATAL: {
      logColor = (String) "\033[0;31m";
      logTag = (String) "[LOG FATAL] =>";
    } break;
  }

  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s %s %s %s\033[0m\n", logColor, logTag, Format, (logLevel == LOG_INFO) ? "" : Context);
  vprintf(logBuffer, args);
  va_end(args);

  if (SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    snprintf(logBuffer, sizeof(logBuffer), "%s %s %s\n", logTag, Format, (logLevel == LOG_INFO) ? "" : Context);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif // PLATFORM_LINUX

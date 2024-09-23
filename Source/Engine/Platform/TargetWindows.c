#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <math.h>

#include "GT/Engine.h"

FGT GEngine;

extern bool ApiWin32Init(IWindowApi* WindowApi);
extern void ApiGdiSwapBuffer();

static FILE* SLogFile = NULL;
static cstring SLogFilePath = "LogFile.txt";
static double SFrameStartTime = 0;
static LARGE_INTEGER STimeFrequency;

static struct {
  HANDLE hConsole;
  int32 defaultAttribute;
} SConsole;

static void InitWin32Console() {
  GEngine.windowApi.bShouldClose = true;
  SConsole.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenInfo;
  GetConsoleScreenBufferInfo(SConsole.hConsole, &consoleScreenInfo);
  SConsole.defaultAttribute = consoleScreenInfo.wAttributes;
}

bool EngineProcess(uint64 Flags, cstring* Args) {
  return false;
}

void EngineInitialize(int32 Width, int32 Height, cstring Title) {
  QueryPerformanceFrequency(&STimeFrequency);
  GEngine.timerApi.engineStartTime = (float)EngineGetTime();
  GEngine.windowApi.bShowCursor = true;  // It is necessary to start as true to avoid bugs with WinApi
  InitWin32Console();
  fopen_s(&SLogFile, SLogFilePath, "w");
  if(ApiWin32Init(&GEngine.windowApi)) {
    GEngine.windowApi.OnWindowCreate(Width, Height, Title);
  }
}

void EngineTerminate() {
  GEngine.windowApi.OnWindowDestroy();
}

bool EngineShouldClose() {
  return GEngine.windowApi.bShouldClose;
}

void EngineShutdown() {
  GEngine.windowApi.bShouldClose = true;
}

void EngineBeginFrame() {
  SFrameStartTime = EngineGetTime();
  GEngine.windowApi.OnWindowUpdate();
}

void EngineEndFrame() {
  ApiGdiSwapBuffer();
  double end = EngineGetTime();
  double delta = end - SFrameStartTime;
  double target = GEngine.timerApi.frameTime;
  double remainingTime = target - delta;

  if(target > 0) {
    if(remainingTime > (delta * 0.9)) {
      EngineWait(remainingTime);
    }
    while((delta = EngineGetTime() - SFrameStartTime) < target) {}
  }

  GEngine.timerApi.deltaTime = delta;
  GEngine.timerApi.frameRate = (uint32)ceil(1.f / delta);
}

void EngineFullscreen(bool bFullscreen) {
  GEngine.windowApi.OnWindowFullscreen(bFullscreen);
}

double EngineGetTime() {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (double)(counter.QuadPart) / STimeFrequency.QuadPart;
}

void EngineSetTargetFPS(uint32 Target) {
  GEngine.timerApi.frameTime = 1.f / Target;
}

void EngineWait(double Milliseconds) {
  Sleep((DWORD)Milliseconds);
}

// Module
void* EngineLoadModule(cstring Name) {
  return LoadLibraryA(Name);
}

void EngineFreeModule(void* Module) {
  if(Module != NULL) {
    FreeLibrary((HMODULE)Module);
  }
}

void* EngineGetFunc(void* Module, cstring Name) {
  return (void*)GetProcAddress((HMODULE)Module, Name);
}

void EngineLoadApi(void* Module, void* Api, cstring* Names, bool bDebugMode) {
  cstring* names = Names;
  void* function = NULL;
  int32 index = 0;

  while(**names) {
    function = EngineGetFunc(Module, *names);
    if(function != NULL) {
      void* addr = (char*)Api + index * sizeof(void*);
      memcpy(addr, &function, sizeof(void*));
      if(bDebugMode) {
        GT_LOG(LOG_INFO, "Function Loaded:%s", *names);
      }
    } else if(bDebugMode) {
      GT_LOG(LOG_INFO, "Function Not Loaded:%s", *names);
    }
    names++;
    index++;
  }
}

void EnginePrintLog(ELogLevel Level, cstring Context, cstring Format, ...) {
  static char logBuffer[BUFFER_LOG_SIZE] = {""};
  static cstring logTag = NULL;
  bool bIsFast = (Level >> 16);
  uint16 logLevel = Level & 0xFFFF;

  enum {
    LC_WHITE = 15,
    LC_GREEN = 10,
    LC_YELLOW = 14,
    LC_RED = 12,
    LC_DARK_RED = 4,
  } color;

  color = LC_WHITE;

  switch(logLevel) {
    case LOG_INFO: {
      logTag = (cstring) "[LOG INFO] =>";
      color = LC_WHITE;
    } break;
    case LOG_SUCCESS: {
      logTag = (cstring) "[LOG SUCCESS] =>";
      color = LC_GREEN;
    } break;
    case LOG_WARNING: {
      logTag = (cstring) "[LOG WARNING] =>";
      color = LC_YELLOW;
    } break;
    case LOG_ERROR: {
      logTag = (cstring) "[LOG ERROR] =>";
      color = LC_RED;
    } break;
    case LOG_FATAL: {
      logTag = (cstring) "[LOG FATAL] =>";
      color = LC_DARK_RED;
    } break;
  }

  SetConsoleTextAttribute(SConsole.hConsole, color);
  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s %s %s\n", logTag, Format, (Level == LOG_INFO) ? "" : Context);
  vprintf(logBuffer, args);
  va_end(args);
  SetConsoleTextAttribute(SConsole.hConsole, SConsole.defaultAttribute);

  if(SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif  // PLATFORM_WINDOWS

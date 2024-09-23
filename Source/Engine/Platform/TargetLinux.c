#ifdef PLATFORM_LINUX
#include "GT/Engine.h"

#include <math.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <time.h>

#define TO_SECONDS 1000000.0

FGT GEngine = {0};

extern bool ApiX11Init(IWindowApi* WindowApi);
extern void ApiEGLSwapBuffer();

static FILE* SLogFile = NULL;
static cstring SLogFilePath = "LogFile.txt";
static double SFrameStartTime = 0;

bool PEngineProcess(uint64 Flags, cstring* Args) {
  return false;
}

void PEngineInitialize(int32 Width, int32 Height, cstring Title) {
  GEngine.timerApi.engineStartTime = PEngineGetTime();
  SLogFile = fopen(SLogFilePath, "w");
  if(ApiX11Init(&GEngine.windowApi)) {
    GEngine.windowApi.OnWindowCreate(Width, Height, Title);
  }
}

void PEngineTerminate() {
  GEngine.windowApi.OnWindowDestroy();
}

bool PEngineShouldClose() {
  return GEngine.windowApi.bShouldClose;
}

void PEngineShutdown() {
  GEngine.windowApi.bShouldClose = true;
}

void PEngineBeginFrame() {
  SFrameStartTime = PEngineGetTime();
  GEngine.windowApi.OnWindowUpdate();
}

void PEngineEndFrame() {
  ApiEGLSwapBuffer();

  double end = PEngineGetTime();
  double delta = end - SFrameStartTime;
  double target = GEngine.timerApi.frameTime;
  double remainingTime = target - delta;

  if(target > 0) {
    if(remainingTime > (delta * 0.9)) {
      PEngineWait(remainingTime);
    }
    while((delta = PEngineGetTime() - SFrameStartTime) < target) {
    }
  }
  GEngine.timerApi.deltaTime = delta;
  GEngine.timerApi.frameRate = ceil(1.0f / delta);
}

void PEngineFullscreen(bool bFullscreen) {
  GEngine.windowApi.OnWindowFullscreen(bFullscreen);
}

double PEngineGetTime() {
  struct timespec timeSpec;
  clock_gettime(CLOCK_MONOTONIC, &timeSpec);
  return (double)timeSpec.tv_sec + (double)timeSpec.tv_nsec / 1000000000.0f;
}

void PEngineSetTargetFPS(uint32 Target) {
  GEngine.timerApi.frameTime = 1.f / Target;
}

void PEngineWait(double Milliseconds) {
  struct timespec time;
  int32 sec = (int32)(Milliseconds / 1000);
  int32 nSeg = (int32)(Milliseconds - sec * 1000) * 1000000.0f;

  time.tv_sec = sec;
  time.tv_nsec = nSeg;
  nanosleep(&time, NULL);
}

// Module
void* PModuleLoad(cstring Name) {
  return dlopen(Name, RTLD_LAZY | RTLD_GLOBAL);
}

void PModuleFree(void* Module) {
  if(Module != NULL) {
    dlclose(Module);
  }
}

void* PModuleGetFunc(void* Module, cstring Name) {
  return dlsym(Module, Name);
}

void PModuleLoadApi(void* Module, void* Api, cstring* Names, bool bDebugMode) {
  cstring* names = Names;
  void* function = NULL;
  int32 index = 0;

  while(**names) {
    function = PModuleGetFunc(Module, *names);
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

void PMemFree(void* Data) {
  if(Data == NULL) { return; }
  free(Data);
}

void* PMemAlloc(uint64 Size) {
  return malloc(Size);
}

void* PMemCalloc(uint32 Count, uint64 Size) {
  return calloc(Count, Size);
}

void* PMemRealloc(void* Data, uint64 Size) {
  return realloc(Data, Size);
}

void* PMemCopy(void* Dest, void* Src, uint64 Size) {
  return memcpy(Dest, Src, Size);
}

void* PMemMove(void* Dest, void* Src, uint64 Size) {
  return memmove(Dest, Src, Size);
}

void* PMemSet(void* Data, int32 Value, uint64 Size){
  return memset(Data, Value, Size);
}

void EnginePrintLog(ELogLevel Level, cstring Context, cstring Format, ...) {
  static char logBuffer[BUFFER_LOG_SIZE] = {""};
  static cstring logTag = NULL;
  static cstring logColor = NULL;
  bool bIsFast = (Level >> 16);
  uint16 logLevel = Level & 0xFFFF;

  switch(logLevel) {
    case LOG_INFO: {
      logColor = (cstring) "\033[0;97m";
      logTag = (cstring) "[LOG INFO] =>";
    } break;
    case LOG_SUCCESS: {
      logColor = (cstring) "\033[0;92m";
      logTag = (cstring) "[LOG SUCCESS] =>";
    } break;
    case LOG_WARNING: {
      logColor = (cstring) "\033[0;93m";
      logTag = (cstring) "[LOG WARNING] =>";
    } break;
    case LOG_ERROR: {
      logColor = (cstring) "\033[0;91m";
      logTag = (cstring) "[LOG ERROR] =>";
    } break;
    case LOG_FATAL: {
      logColor = (cstring) "\033[0;31m";
      logTag = (cstring) "[LOG FATAL] =>";
    } break;
  }

  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s %s %s %s\033[0m\n", logColor, logTag, Format, (logLevel == LOG_INFO) ? "" : Context);
  vprintf(logBuffer, args);
  va_end(args);

  if(SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    snprintf(logBuffer, sizeof(logBuffer), "%s %s %s\n", logTag, Format, (logLevel == LOG_INFO) ? "" : Context);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif  // PLATFORM_LINUX

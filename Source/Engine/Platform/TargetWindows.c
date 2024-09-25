#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <math.h>

#include "GT/Engine.h"
#include "Renderer.h"

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

bool PEngineProcess(uint64 Flags, cstring* Args) {
  return false;
}

void PEngineInitialize(int32 Width, int32 Height, cstring Title) {
  QueryPerformanceFrequency(&STimeFrequency);
  GEngine.timerApi.engineStartTime = (float)PEngineGetTime();
  GEngine.windowApi.bShowCursor = true;  // It is necessary to start as true to avoid bugs with WinApi
  InitWin32Console();
  fopen_s(&SLogFile, SLogFilePath, "w");
  if(ApiWin32Init(&GEngine.windowApi)) {
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
  RClearBuffers();
}

void PEngineEndFrame() {
  ApiGdiSwapBuffer();
  double end = PEngineGetTime();
  double delta = end - SFrameStartTime;
  double target = GEngine.timerApi.frameTime;
  double remainingTime = target - delta;

  if(target > 0) {
    if(remainingTime > (delta * 0.9)) {
      PEngineWait(remainingTime);
    }
    while((delta = PEngineGetTime() - SFrameStartTime) < target) {}
  }

  GEngine.timerApi.deltaTime = delta;
  GEngine.timerApi.frameRate = (uint32)ceil(1.f / delta);
}

void PEngineFullscreen(bool bFullscreen) {
  GEngine.windowApi.OnWindowFullscreen(bFullscreen);
}

double PEngineGetTime() {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (double)(counter.QuadPart) / STimeFrequency.QuadPart;
}

void PEngineSetTargetFPS(uint32 Target) {
  GEngine.timerApi.frameTime = 1.f / Target;
}

void PEngineWait(double Milliseconds) {
  Sleep((DWORD)Milliseconds);
}

// Module
void* PModuleLoad(cstring Name) {
  return LoadLibraryA(Name);
}

void PModuleFree(void* Module) {
  if(Module != NULL) {
    FreeLibrary((HMODULE)Module);
  }
}

void* PModuleGetFunc(void* Module, cstring Name) {
  return (void*)GetProcAddress((HMODULE)Module, Name);
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

// Memory
void PMemFree(void* Data) {
  if(Data == NULL) { return; }
  free(Data);
}

void* PMemMalloc(uint64 Size) {
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

void* PMemSet(void* Data, int32 Value, uint64 Size) {
  return memset(Data, Value, Size);
}

// Files
bool PReadTextFile(cstring Path, char** Buffer, uint64* FileSize, uint64 ExtraSize) {
  FILE* file = fopen(Path, "r");
  if(file == NULL) {
    cstring path = strstr(Path, STR(GAME_NAME));
    path = (path == NULL) ? Path : path;
    GT_LOG(LOG_ALERT, "Does not exist -> %s", path);
    return false;
  }

  fseek(file, 0, SEEK_END);
  uint64 size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if(FileSize != NULL) *FileSize = (size + ExtraSize);

  *Buffer = PMemCalloc(1, size + ExtraSize + 1);

  fread(*Buffer, 1, size, file);
  (*Buffer)[size + ExtraSize] = '\0';

  fclose(file);
  return true;
}

bool PReadBinaryFile(cstring Path, char** Buffer, uint64* FileSize, uint64 ExtraSize) {
  FILE* file = fopen(Path, "rb");
  if(file == NULL) {
    cstring path = strstr(Path, STR(GAME_NAME));
    path = (path == NULL) ? Path : path;
    GT_LOG(LOG_ALERT, "Does not exist -> %s", path);
    return false;
  }

  fseek(file, 0, SEEK_END);
  uint64 size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if(FileSize != NULL) *FileSize = (size + ExtraSize);

  *Buffer = PMemCalloc(1, size + ExtraSize);

  uint64 readBytes = fread(*Buffer, 1, size, file);

  if(size != readBytes) {
    GT_LOG(LOG_ALERT, "Didn't read everything! %llu/%llu -> %s", readBytes, size, Path);
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

void EnginePrintLog(ELogLevel Level, cstring FuncName, cstring Context, cstring Format, ...) {
  static char logBuffer[BUFFER_LOG_SIZE] = {""};
  static cstring logTag = NULL;
  cstring context = Context;
  cstring funcName = FuncName;
  bool bIsFast = (Level >> 16);
  uint16 logLevel = Level & 0xFFFF;

  enum {
    LC_WHITE = 7,
    LC_GREEN = 10,
    LC_YELLOW = 14,
    LC_RED = 12,
    LC_DARK_RED = 4,
    LC_DARK_YELLOW = 6
  } color;

  color = LC_WHITE;

  switch(logLevel) {
    case LOG_INFO: {
      logTag = (cstring) "[LOG INFO] =>";
      color = LC_WHITE;
      funcName = (cstring) "";
      context = (cstring) "";
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
    case LOG_ALERT: {
      logTag = (cstring) "[LOG ALERT] =>";
      color = LC_DARK_YELLOW;
      context = (cstring) "";
    } break;
  }

  cstring it = (logLevel == LOG_INFO) ? "" : "()";
  SetConsoleTextAttribute(SConsole.hConsole, color);
  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s%s%s %s %s\n", logTag, funcName, it, Format, context);
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

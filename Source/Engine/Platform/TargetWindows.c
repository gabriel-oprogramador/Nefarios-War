#include "CoreMinimal.h"
#ifdef PLATFORM_WINDOWS
#include <windows.h>

#include "GT/Engine.h"

extern bool ApiWin32Init(IWindowApi* Api);
extern bool ApiWglInit(IGraphicApi* Api);

static struct {
  HANDLE hConsole;
  int32 consoleDefaultAttribute;
  LARGE_INTEGER timerFrequency;
  FILE* logFile;
} SWinOS;

void PPlatformInitialize() {
  QueryPerformanceFrequency(&SWinOS.timerFrequency);
  SWinOS.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenInfo;
  GetConsoleScreenBufferInfo(SWinOS.hConsole, &consoleScreenInfo);
  SWinOS.consoleDefaultAttribute = consoleScreenInfo.wAttributes;
  SWinOS.logFile = fopen(GT_LOG_FILE_PATH, "w");

  if(ApiWin32Init(&GEngine.windowApi)) {
    if(ApiWglInit(&GEngine.graphicApi)) {
      // Bla bla
    }
    return;
  }
}

void PPlatformTerminate() {
}

double PGetTime() {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (double)(counter.QuadPart) / SWinOS.timerFrequency.QuadPart;
}

void PWait(double Milliseconds) {
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
bool PReadTextFile(cstring Path, char** OutBuffer, uint64* FileSize, uint64 ExtraSize) {
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

  *OutBuffer = PMemCalloc(1, size + ExtraSize + 1);

  fread(*OutBuffer, sizeof(char), size, file);
  (*OutBuffer)[size + ExtraSize] = '\0';

  fclose(file);
  return true;
}

bool PReadBinaryFile(cstring Path, char** OutBuffer, uint64* FileSize, uint64 ExtraSize) {
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

  *OutBuffer = PMemCalloc(1, size + ExtraSize);

  uint64 readBytes = fread(*OutBuffer, sizeof(char), size, file);

  if(size != readBytes) {
    GT_LOG(LOG_ALERT, "Didn't read everything! %llu/%llu -> %s", readBytes, size, Path);
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

bool PWriteTextFile(cstring Path, const char* InBuffer, bool bAppend) {
  cstring mode = (bAppend) ? "a" : "w";
  FILE* file = fopen(Path, mode);
  if(file == NULL) {
    GT_LOG(LOG_ALERT, "File not exist -> %s", Path);
    return false;
  }
  if(InBuffer == NULL) {
    GT_LOG(LOG_ALERT, "InBuffer is NULL -> %s", Path);
    fclose(file);
    return false;
  }

  fputs(InBuffer, file);
  fclose(file);
  return true;
}

bool PWriteBinaryFile(cstring Path, const char* InBuffer, uint64 Size, bool bAppend) {
  cstring mode = (bAppend) ? "ab" : "wb";
  FILE* file = fopen(Path, mode);
  if(file == NULL) {
    GT_LOG(LOG_ALERT, "File not exist -> %s", Path);
    return false;
  }
  if(InBuffer == NULL) {
    GT_LOG(LOG_ALERT, "InBuffer is NULL -> %s", Path);
    fclose(file);
    return false;
  }

  if(fwrite(InBuffer, sizeof(char), Size, file) != Size) {
    GT_LOG(LOG_ALERT, "Writing not completed.");
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

  cstring isFunc = (logLevel == LOG_INFO) ? "" : "()";
  cstring isSpace = (logLevel != LOG_INFO) ? " " : "";
  SetConsoleTextAttribute(SWinOS.hConsole, color);
  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s%s%s%s %s %s\n", logTag, isSpace, funcName, isFunc, Format, context);
  vprintf(logBuffer, args);
  va_end(args);
  SetConsoleTextAttribute(SWinOS.hConsole, SWinOS.consoleDefaultAttribute);

  if(SWinOS.logFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    vfprintf(SWinOS.logFile, logBuffer, args);
    fflush(SWinOS.logFile);
    va_end(args);
  }
}
#endif  // PLATFORM_WINDOWS

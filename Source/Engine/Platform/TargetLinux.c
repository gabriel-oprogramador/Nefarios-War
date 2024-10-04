#include "CoreMinimal.h"
#ifdef PLATFORM_LINUX
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>

#include "GT/Engine.h"
#include "GT/Platform.h"

#define TO_SECONDS 1000000.0

extern bool ApiX11Init(IWindowApi* Api);
extern bool ApiEGLInit(IGraphicApi* Api);

static FILE* SLogFile = NULL;

void PPlatformInitialize() {
  SLogFile = fopen(GT_LOG_FILE_PATH, "w");
  if(ApiX11Init(&GEngine.windowApi)){
    if(ApiEGLInit(&GEngine.graphicApi)){

    }
  }
}

void PPlatformTerminate() {
}

// Returns in Seconds
double PGetTime() {
  struct timespec timeSpec;
  clock_gettime(CLOCK_MONOTONIC, &timeSpec);
  return (double)timeSpec.tv_sec + (double)timeSpec.tv_nsec / 1000000000.0f;
}

void PWait(double Milliseconds) {
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
  static cstring logColor = NULL;
  cstring context = Context;
  cstring funcName = FuncName;
  bool bIsFast = (Level >> 16);
  uint16 logLevel = Level & 0xFFFF;

  switch(logLevel) {
    case LOG_INFO: {
      logColor = (cstring) "\033[0;97m";
      logTag = (cstring) "[LOG INFO] => ";
      funcName = (cstring) "";
      context = (cstring) "";
    } break;
    case LOG_SUCCESS: {
      logColor = (cstring) "\033[0;92m";
      logTag = (cstring) "[LOG SUCCESS] => ";
    } break;
    case LOG_WARNING: {
      logColor = (cstring) "\033[0;93m";
      logTag = (cstring) "[LOG WARNING] => ";
    } break;
    case LOG_ERROR: {
      logColor = (cstring) "\033[0;91m";
      logTag = (cstring) "[LOG ERROR] => ";
    } break;
    case LOG_FATAL: {
      logColor = (cstring) "\033[0;31m";
      logTag = (cstring) "[LOG FATAL] => ";
    } break;
    case LOG_ALERT: {
      logColor = (cstring) "\033[0;33m";
      logTag = (cstring) "[LOG ALERT] => ";
      context = (cstring) "";
    } break;
  }

  cstring it = (logLevel == LOG_INFO) ? "" : "() ";
  va_list args;
  va_start(args, Format);
  snprintf(logBuffer, sizeof(logBuffer), "%s%s%s%s%s %s\033[0m\n", logColor, logTag, funcName, it, Format, context);
  vprintf(logBuffer, args);
  va_end(args);

  if(SLogFile != NULL && bIsFast != 1) {
    va_start(args, Format);
    snprintf(logBuffer, sizeof(logBuffer), "%s%s%s%s %s\n", logTag, funcName, it, Format, context);
    vfprintf(SLogFile, logBuffer, args);
    va_end(args);
  }
}

#endif  // PLATFORM_LINUX

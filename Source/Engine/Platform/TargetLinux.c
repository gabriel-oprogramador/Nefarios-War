#ifdef PLATFORM_LINUX
#include <dlfcn.h>
#include "Engine.h"

extern Bool ApiX11Init(FGT* Engine);

FGT GEngine = {0};

static FILE* SLogFile = NULL;
static String SLogFilePath = "LogFile.txt";

Void EngineInit(Int32 Width, Int32 Height, String Title) {
  SLogFile = fopen(SLogFilePath, "w");
  if(ApiX11Init(&GEngine)){
    GT_LOG(LOG_INFO, "PLATFORM: X11 API Initialized");
    GEngine.WindowCreate(Width, Height, Title);
  }
}

Bool EngineShouldClose() {
  return GEngine.WindowShouldClose();
}

Void EngineShutdown() {
  GEngine.WindowDestroy();
}

Void EngineBeginFrame() {
  GEngine.WindowUpdate();
}

Void EngineEndFrame() {
}

// Module
Void* EngineLoadModule(String Name){
  return dlopen(Name, RTLD_LAZY | RTLD_GLOBAL);
}

Void EngineFreeModule(Void* Module){
  if(Module != NULL){
    dlclose(Module);
  }
}

Void* EngineGetFunc(Void* Module, String Name){
  return dlsym(Module, Name);
}

Void EngineLoadApi(Void* Module, Void* Api, String* Names, Bool bDebugMode){
  String* names = Names;
  Void* function = NULL;
  Int32 index = 0;

  while(**names){
    function = EngineGetFunc(Module, *names);
    if(function != NULL){
      Void* addr = (Char*)Api + index * sizeof(Void*);
      memcpy(addr, &function, sizeof(Void*));
      if(bDebugMode){
        GT_LOG(LOG_INFO, "Function Loaded:%s", *names);
      }
    }else if(bDebugMode){
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

#pragma once
#include "CoreMinimal.h"

ENGINE_API void PPlatformInitialize();
ENGINE_API void PPlatformTerminate();

// Returns in Seconds
ENGINE_API double PGetTime();
ENGINE_API void PWait(double Milliseconds);

// Module
ENGINE_API void* PModuleLoad(cstring Name);
ENGINE_API void PModuleFree(void* Module);
ENGINE_API void* PModuleGetFunc(void* Module, cstring Name);
ENGINE_API void PModuleLoadApi(void* Module, void* Api, cstring* Names, bool bDebugMode);

// Memory
ENGINE_API void PMemFree(void* Data);
ENGINE_API void* PMemMalloc(uint64 Size);
ENGINE_API void* PMemCalloc(uint32 Count, uint64 Size);
ENGINE_API void* PMemRealloc(void* Data, uint64 Size);
ENGINE_API void* PMemCopy(void* Dest, void* Src, uint64 Size);
ENGINE_API void* PMemMove(void* Dest, void* Src, uint64 Size);
ENGINE_API void* PMemSet(void* Data, int32 Value, uint64 Size);

// Files
ENGINE_API bool PReadTextFile(cstring Path, char** OutBuffer, uint64* FileSize, uint64 ExtraSize);
ENGINE_API bool PReadBinaryFile(cstring Path, char** OutBuffer, uint64* FileSize, uint64 ExtraSize);
ENGINE_API bool PWriteTextFile(cstring Path, const char* InBuffer, bool bAppend);
ENGINE_API bool PWriteBinaryFile(cstring Path, const char* InBuffer, uint64 Size, bool bAppend);

#define MOUSE_LEFT_CODE     0b0001  // 0x1 D1
#define MOUSE_MIDDLE_CODE   0b0010  // 0x2 D2
#define MOUSE_RIGHT_CODE    0b0011  // 0x3 D3
#define MOUSE_FORWARD_CODE  0b0100  // 0x4 D4
#define MOUSE_BACKWARD_CODE 0b0101  // 0x5 D5

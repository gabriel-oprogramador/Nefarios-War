#pragma once
#include "CoreMinimal.h"

ENGINE_API void FRendererInitialize(ERendererApi Renderer);
ENGINE_API void FRendererTerminate();
ENGINE_API void FSetClearColor(FColor Color);
ENGINE_API void FClearBuffers();
ENGINE_API void FSetViewport(FRect Viewport);

// Shader Program
ENGINE_API FShader FShaderLoad(cstring VertexPath, cstring FragmentPath);
ENGINE_API FShader FShaderCreate(cstring VertexSource, cstring FragmentSource);
ENGINE_API void FShaderFree(FShader Shader);

// Primitives
ENGINE_API FPrimitive FGetPrimitive(EPrimitiveShape Shape);

//TODO:Refactor to add position!
ENGINE_API void FDrawPrimitive(FPrimitive Primitive, FColor Color);

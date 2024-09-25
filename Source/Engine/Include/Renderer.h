#pragma once
#include "CoreMinimal.h"

ENGINE_API void RRendererInitialize();
ENGINE_API void RRendererTerminate();
ENGINE_API void RSetClearColor(FColor Color);
ENGINE_API void RClearBuffers();
ENGINE_API void RSetViewport(FRect Viewport);

// Shader Program
ENGINE_API RShader RShaderLoad(cstring VertexPath, cstring FragmentPath);
ENGINE_API RShader RShaderCreate(cstring VertexSource, cstring FragmentSource);
ENGINE_API void RShaderFree(RShader Shader);

// Primitives
ENGINE_API RPrimitive RGetPrimitive(EPrimitiveShape Shape);

//TODO:Refactor to add position!
ENGINE_API void RDrawPrimitive(RPrimitive Primitive, FColor Color);

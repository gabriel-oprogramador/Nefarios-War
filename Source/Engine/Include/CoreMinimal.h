#pragma once
/*Game Template or GT-Engine is my study-focused engine written in C made from scratch to create simple games.*/
/*Original Project by Gabriel Oliveira Prudente @gabriel_Oprogramador*/

#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GT/EngineTypes.h"

#ifdef __cplusplus
#define ENGINE_API extern "C"
#else
#define ENGINE_API extern
#endif  //__cplusplus

#ifdef _MSC_VER
#define FORCE_INLINE static __forceinline
#else
#define FORCE_INLINE static inline
#endif  // _MSC_VER

ENGINE_API void EnginePrintLog(ELogLevel Level, cstring FuncName, cstring Context, cstring Format, ...);

#define NO_EXPAND(a)                   #a
#define STR(a)                         NO_EXPAND(a)
#define CONCAT(A, B)                   A##B
#define FIND_CONFIG(ConfigPath)        STR(CONFIG_PATH) ConfigPath
#define FIND_ASSET(AssetPath)          STR(CONTENT_PATH) AssetPath
#define FN_LOG                         __func__
#define CONTEXT_LOG                    "-> " STR(__FILE__) ":" STR(__LINE__)
#define BUFFER_SMALL                   256
#define BUFFER_LOG_SIZE                2048
#define GT_LOG(Level, Format, ...)     EnginePrintLog(Level, FN_LOG, CONTEXT_LOG, Format, ##__VA_ARGS__)
#define GT_LOGTEMP(Level, Format, ...) EnginePrintLog((ELogLevel)((1 << 16) | Level), FN_LOG, CONTEXT_LOG, Format, ##__VA_ARGS__)
#define GT_LOG_FILE_PATH               "LogFile.txt"
#define FCOLOR_GL(FCOLOR)              FCOLOR.r, FCOLOR.g, FCOLOR.b, FCOLOR.a
#define FCOLOR_IS_EQUAL(C1, C2)        (C1.r == C2.r && C1.g == C2.g && C1.b == C2.b && C1.a == C2.a)
#define CONFIG_DIR                     "Config/"
#define CONTENT_DIR                    "Content/"

#if defined(__cplusplus)
#define CLITERAL(type) type
#else
#define CLITERAL(type) (type)
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// clang-format off
// Red
#define COLOR_RED            CLITERAL(FColor){ 1.0f, 0.0f, 0.0f, 1.0f }
#define COLOR_RED_LIGHT      CLITERAL(FColor){ 1.0f, 0.5f, 0.5f, 1.0f }
#define COLOR_RED_DARK       CLITERAL(FColor){ 0.5f, 0.0f, 0.0f, 1.0f }

// Green
#define COLOR_GREEN          CLITERAL(FColor){ 0.0f, 1.0f, 0.0f, 1.0f }
#define COLOR_GREEN_LIGHT    CLITERAL(FColor){ 0.5f, 1.0f, 0.5f, 1.0f }
#define COLOR_GREEN_DARK     CLITERAL(FColor){ 0.0f, 0.5f, 0.0f, 1.0f }

// Blue
#define COLOR_BLUE           CLITERAL(FColor){ 0.0f, 0.0f, 1.0f, 1.0f }
#define COLOR_BLUE_LIGHT     CLITERAL(FColor){ 0.5f, 0.5f, 1.0f, 1.0f }
#define COLOR_BLUE_DARK      CLITERAL(FColor){ 0.0f, 0.0f, 0.5f, 1.0f }

// Yellow
#define COLOR_YELLOW         CLITERAL(FColor){ 1.0f, 1.0f, 0.0f, 1.0f }
#define COLOR_YELLOW_LIGHT   CLITERAL(FColor){ 1.0f, 1.0f, 0.5f, 1.0f }
#define COLOR_YELLOW_DARK    CLITERAL(FColor){ 0.5f, 0.5f, 0.0f, 1.0f }

// Cyan
#define COLOR_CYAN           CLITERAL(FColor){ 0.0f, 1.0f, 1.0f, 1.0f }
#define COLOR_CYAN_LIGHT     CLITERAL(FColor){ 0.5f, 1.0f, 1.0f, 1.0f }
#define COLOR_CYAN_DARK      CLITERAL(FColor){ 0.0f, 0.5f, 0.5f, 1.0f }

// Magenta
#define COLOR_MAGENTA        CLITERAL(FColor){ 1.0f, 0.0f, 1.0f, 1.0f }
#define COLOR_MAGENTA_LIGHT  CLITERAL(FColor){ 1.0f, 0.5f, 1.0f, 1.0f }
#define COLOR_MAGENTA_DARK   CLITERAL(FColor){ 0.5f, 0.0f, 0.5f, 1.0f }

// Orange
#define COLOR_ORANGE         CLITERAL(FColor){ 1.0f, 0.65f, 0.0f, 1.0f }
#define COLOR_ORANGE_LIGHT   CLITERAL(FColor){ 1.0f, 0.8f, 0.5f, 1.0f }
#define COLOR_ORANGE_DARK    CLITERAL(FColor){ 0.5f, 0.32f, 0.0f, 1.0f }

// Purple
#define COLOR_PURPLE         CLITERAL(FColor){ 0.5f, 0.0f, 0.5f, 1.0f }
#define COLOR_PURPLE_LIGHT   CLITERAL(FColor){ 0.75f, 0.5f, 0.75f, 1.0f }
#define COLOR_PURPLE_DARK    CLITERAL(FColor){ 0.25f, 0.0f, 0.25f, 1.0f }

// Brown
#define COLOR_BROWN          CLITERAL(FColor){ 0.65f, 0.16f, 0.16f, 1.0f }
#define COLOR_BROWN_LIGHT    CLITERAL(FColor){ 0.82f, 0.41f, 0.41f, 1.0f }
#define COLOR_BROWN_DARK     CLITERAL(FColor){ 0.33f, 0.08f, 0.08f, 1.0f }

// Pink
#define COLOR_PINK           CLITERAL(FColor){ 1.0f, 0.75f, 0.8f, 1.0f }
#define COLOR_PINK_LIGHT     CLITERAL(FColor){ 1.0f, 0.88f, 0.88f, 1.0f }
#define COLOR_PINK_DARK      CLITERAL(FColor){ 0.5f, 0.38f, 0.4f, 1.0f }

// Gold
#define COLOR_GOLD           CLITERAL(FColor){ 1.0f, 0.84f, 0.0f, 1.0f }
#define COLOR_GOLD_LIGHT     CLITERAL(FColor){ 1.0f, 0.92f, 0.5f, 1.0f }
#define COLOR_GOLD_DARK      CLITERAL(FColor){ 0.5f, 0.42f, 0.0f, 1.0f }

// Silver
#define COLOR_SILVER         CLITERAL(FColor){ 0.75f, 0.75f, 0.75f, 1.0f }
#define COLOR_SILVER_LIGHT   CLITERAL(FColor){ 0.88f, 0.88f, 0.88f, 1.0f }
#define COLOR_SILVER_DARK    CLITERAL(FColor){ 0.38f, 0.38f, 0.38f, 1.0f }

// Lime
#define COLOR_LIME           CLITERAL(FColor){ 0.0f, 1.0f, 0.0f, 1.0f }
#define COLOR_LIME_LIGHT     CLITERAL(FColor){ 0.5f, 1.0f, 0.5f, 1.0f }
#define COLOR_LIME_DARK      CLITERAL(FColor){ 0.0f, 0.5f, 0.0f, 1.0f }

// Gray
#define COLOR_GRAY           CLITERAL(FColor){ 0.5f, 0.5f, 0.5f, 1.0f }
#define COLOR_GRAY_LIGHT     CLITERAL(FColor){ 0.7f, 0.7f, 0.7f, 1.0f }
#define COLOR_GRAY_DARK      CLITERAL(FColor){ 0.3f, 0.3f, 0.3f, 1.0f }

// Sky Blue
#define COLOR_SKY_BLUE       CLITERAL(FColor){ 0.53f, 0.81f, 0.92f, 1.0f }
#define COLOR_SKY_BLUE_LIGHT CLITERAL(FColor){ 0.73f, 0.91f, 1.0f, 1.0f }
#define COLOR_SKY_BLUE_DARK  CLITERAL(FColor){ 0.3f, 0.6f, 0.8f, 1.0f  }

// White
#define COLOR_WHITE          CLITERAL(FColor){ 1.0f, 1.0f, 1.0f, 1.0f }
#define COLOR_WHITE_LIGHT    CLITERAL(FColor){ 1.0f, 1.0f, 1.0f, 1.0f }
#define COLOR_WHITE_DARK     CLITERAL(FColor){ 0.8f, 0.8f, 0.8f, 1.0f }

// Chrominum
#define COLOR_CHROMIUM       CLITERAL(FColor){ 0.8f, 0.8f, 0.8f, 1.0f }
#define COLOR_CHROMIUM_LIGHT CLITERAL(FColor){ 0.9f, 0.9f, 0.9f, 1.0f }
#define COLOR_CHROMIUM_DARK  CLITERAL(FColor){ 0.6f, 0.6f, 0.6f, 1.0f }

// Copper
#define COLOR_COPPER         CLITERAL(FColor){ 0.72f, 0.45f, 0.2f, 1.0f }
#define COLOR_COPPER_LIGHT   CLITERAL(FColor){ 0.85f, 0.55f, 0.25f, 1.0f }
#define COLOR_COPPER_DARK    CLITERAL(FColor){ 0.6f, 0.35f, 0.15f, 1.0f }

// Iron
#define COLOR_IRON           CLITERAL(FColor){ 0.5f, 0.5f, 0.5f, 1.0f }
#define COLOR_IRON_LIGHT     CLITERAL(FColor){ 0.7f, 0.7f, 0.7f, 1.0f }
#define COLOR_IRON_DARK      CLITERAL(FColor){ 0.3f, 0.3f, 0.3f, 1.0f }

// Black
#define COLOR_BLACK          CLITERAL(FColor){ 0.0f, 0.0f, 0.0f, 1.0f }
#define COLOR_BLACK_LIGHT    CLITERAL(FColor){ 0.2f, 0.2f, 0.2f, 1.0f }
#define COLOR_BLACK_DARK     CLITERAL(FColor){ 0.0f, 0.0f, 0.0f, 1.0f }

// BLANK
#define COLOR_BLANK          CLITERAL(FColor){ 0.0f, 0.0f, 0.0f, 0.0f }
// clang-format on

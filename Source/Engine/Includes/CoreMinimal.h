#pragma once
/*Game Template or GT-Engine is my study-focused engine written in C made from scratch to create simple games.*/
/*Original Project by Gabriel Oliveira Prudente @gabriel_Oprogramador*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#define NO_EXPAND(a) #a
#define STR(a) NO_EXPAND(a)
#define FIND_CONFIG(ConfigPath) STR(CONFIG_PATH) ConfigPath
#define FIND_ASSET(AssetPath) STR(CONTENT_PATH) AssetPath

typedef void Void;
typedef bool Bool;
typedef char Char;
typedef short Int16;
typedef int Int32;
typedef long long Int64;
typedef unsigned char UChar;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;
typedef float Float;
typedef double Double;
typedef const char* String;


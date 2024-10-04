GAME = $(notdir $(CURDIR))

#If you use Visual Studio Code, switch to g++!
CC = clang
CCXX = clang++
USE_CLANG = true

CONFIG_DIR = $(abspath Config/)
CONTENT_DIR = $(abspath Content/)

INCLUDES = -ISource/Engine -ISource/Engine/Include -ISource/Engine/Game -ISource/Game/Include
DEFINES = -DGAME_NAME=$(GAME) -DRENDERER_OPENGL
FLAGS =
KIND =
CFLAGS =
CXXFLAGS =

ifeq ($(USE_CLANG), true)
	CLANG_MJ = -MJ $@.json
	CLANG_FILE = compile_commands.json
define CLANG_GENERATOR_CC
	echo '[' > $(CLANG_FILE)
	cat $(OBJ_ENGINE_OUTPUT)/*.o.json >> $(CLANG_FILE)
	cat $(OBJ_GAME_OUTPUT)/*.o.json >> $(CLANG_FILE)
	echo ']' >> $(CLANG_FILE)
endef
endif

define MAKE_PACKAGE
mkdir -p $(BIN_OUTPUT)/Data
cp -r $(CONFIG_DIR) $(BIN_OUTPUT)/Data/
cp -r $(CONTENT_DIR) $(BIN_OUTPUT)/Data/
endef

ifeq ($(OS), Windows_NT)
	DEFINES += -DPLATFORM_WINDOWS
	CFLAGS += -std=c99
	CXXFLAGS += -std=c++17
	PLATFORM = Windows
	EXTENSION = .exe
	KIND = -mwindows
	LIBS =
	LIBS_PATH =
else ifeq ($(shell uname), Linux)
	DEFINES += -DPLATFORM_LINUX
	CFLAGS += -std=gnu99
	CXXFLAGS += -std=gnu++17
	PLATFORM = Linux
	LIBS = -lm
	LIBS_PATH =
endif

config = debug
ifeq ($(config), debug)
	DEFINES += -DDEBUG_MODE -DCONFIG_PATH=$(CONFIG_DIR)/ -DCONTENT_PATH=$(CONTENT_DIR)/
	FLAGS += -g -Wall
	CONFIG = Debug
	KIND =
	MAKE_PACKAGE =
else
	DEFINES += -DRELEASE_MODE -DCONFIG_PATH=./Data/Config/ -DCONTENT_PATH=./Data/Content/
	FLAGS += -O3
	CONFIG = Release
endif

BUILD_DIR = Build
INTERMEDIATE_DIR = Intermediate
OBJ_DIR = Build

# Engine Files C | Game File C CXX

BIN_OUTPUT = $(BUILD_DIR)/$(CONFIG)
OBJ_OUTPUT = $(INTERMEDIATE_DIR)/$(OBJ_DIR)/$(CONFIG)
OBJ_ENGINE_OUTPUT = $(OBJ_OUTPUT)/Engine
OBJ_GAME_OUTPUT = $(OBJ_OUTPUT)/Game

define FIND_ENGINE_FILES
$(wildcard Source/Engine/*.c)
$(wildcard Source/Engine/*/*.c)
$(wildcard Source/Engine/*/*/*.c)
$(wildcard Source/Engine/*/*/*/*.c)
endef

define FIND_GAME_C_FILES
$(wildcard Source/Game/*.c)
$(wildcard Source/Game/*/*.c)
$(wildcard Source/Game/*/*/*.c)
$(wildcard Source/Game/*/*/*/*.c)
endef

define FIND_GAME_CXX_FILES
$(wildcard Source/Game/*.cpp)
$(wildcard Source/Game/*/*.cpp)
$(wildcard Source/Game/*/*/*.cpp)
$(wildcard Source/Game/*/*/*/*.cpp)
endef

OBJ_ENGINE = $(addprefix $(OBJ_ENGINE_OUTPUT)/, $(notdir $(FIND_ENGINE_FILES:.c=.o)))
OBJ_GAME_C = $(addprefix $(OBJ_GAME_OUTPUT)/, $(notdir $(FIND_GAME_C_FILES:.c=.o)))
OBJ_GAME_CXX = $(addprefix $(OBJ_GAME_OUTPUT)/, $(notdir $(FIND_GAME_CXX_FILES:.cpp=.o)))
OBJ = $(OBJ_ENGINE) $(OBJ_GAME_C) $(OBJ_GAME_CXX)

all: $(GAME)

install:
	@echo 'Starting Build => Compiler:$(CC) | Platform:$(PLATFORM) | Config:$(CONFIG)'
	@mkdir -p $(BIN_OUTPUT)
	@mkdir -p $(OBJ_OUTPUT)
	@mkdir -p $(CONFIG_DIR)
	@mkdir -p $(CONTENT_DIR)
	@mkdir -p $(OBJ_ENGINE_OUTPUT)
	@mkdir -p $(OBJ_GAME_OUTPUT)

clean:
	@echo 'Cleaning Everything'
	@rm -rf $(BUILD_DIR)
	@rm -rf $(INTERMEDIATE_DIR)

run: $(GAME)
	@echo 'Running -> $(BIN_OUTPUT)/$(GAME)$(EXTENSION)'
	@./$(BIN_OUTPUT)/$(GAME)$(EXTENSION)

rebuild: clean run

# Linker Target //================================================//
$(GAME): install $(OBJ)
	@echo 'Linking -> $(BIN_OUTPUT)/$(GAME)$(EXTENSION)'
	@$(CXX) $(KIND) $(OBJ) $(FLAGS) $(LIBS_PATH) $(LIBS) -o$(BIN_OUTPUT)/$(GAME)$(EXTENSION)
	@$(CLANG_GENERATOR_CC)
	@$(MAKE_PACKAGE)

# Engine Target //===============================================//
$(OBJ_ENGINE_OUTPUT)/%.o: Source/Engine/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_ENGINE_OUTPUT)/%.o: Source/Engine/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_ENGINE_OUTPUT)/%.o: Source/Engine/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_ENGINE_OUTPUT)/%.o: Source/Engine/*/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@

# Game Target //================================================//
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CLANG_MJ) $< $(FLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o$@

# Game C Plus Plus
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/%.cpp
	@echo 'Compiling >> $<'
	@$(CCXX) $(CLANG_MJ) $< $(FLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/%.cpp
	@echo 'Compiling >> $<'
	@$(CCXX) $(CLANG_MJ) $< $(FLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/*/%.cpp
	@echo 'Compiling >> $<'
	@$(CCXX) $(CLANG_MJ) $< $(FLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_GAME_OUTPUT)/%.o: Source/Game/*/*/*/%.cpp
	@echo 'Compiling >> $<'
	@$(CCXX) $(CLANG_MJ) $< $(FLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o$@

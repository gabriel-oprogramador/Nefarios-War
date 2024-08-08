GAME = $(notdir $(CURDIR))

#If you use Visual Studio Code, switch to gcc!
CC = clang

CONFIG_DIR = $(abspath Config/)
CONTENT_DIR = $(abspath Content/)

INCLUDES = -ISource/Engine -ISource/Engine/Includes -ISource/Engine/Game -ISource/Game/Includes
DEFINES = -DGAME_NAME=$(GAME)
FLAGS =

define MAKE_PAKAGE
mkdir -p $(BIN_OUTPUT)/Data
cp -r $(CONFIG_DIR) $(BIN_OUTPUT)/Data/
cp -r $(CONTENT_DIR) $(BIN_OUTPUT)/Data/
endef

config = debug
ifeq ($(config), debug)
	DEFINES += -DDEBUG_MODE -DCONFIG_PATH=$(CONFIG_DIR)/ -DCONTENT_PATH=$(CONTENT_DIR)/
	FLAGS += -g -Wall
	CONFIG = Debug
	MAKE_PAKAGE =
else
	DEFINES += -DRELEASE_MODE -DCONFIG_PATH=./Data/Config/ -DCONTENT_PATH=./Data/Content/
	FLAGS += -O3
	CONFIG = Release
endif

ifeq ($(OS), Windows_NT)
	DEFINES += -DPLATFORM_WINDOWS
	FLAGS += -std=c99
	PLATFORM = Windows
	EXTENSION = .exe
	LIBS =
	LIBS_PATH =
else ifeq (shell uname, Linux)
	DEFINES += -DPLATFORM_LINUX
	FLAGS += -std=gnu99
	PLATFORM = Linux
	LIBS = -lm
	LIBS_PATH =
endif

ifeq ($(CC), clang)
	CL_GENERATE = @sed -e '1s/^/[\n''/' -e '$$s/,$$/\n'']/' $(JSON) > compile_commands.json
	CL_COMPILER = -MJ$@.json
endif

BUILD_DIR = Build
INTERMEDIATE_DIR = Intermediate
OBJ_DIR = Build

BIN_OUTPUT = $(BUILD_DIR)/$(CONFIG)
OBJ_OUTPUT = $(INTERMEDIATE_DIR)/$(OBJ_DIR)/$(CONFIG)

define FIND_C_FILES
$(wildcard Source/*.c)
$(wildcard Source/*/*.c)
$(wildcard Source/*/*/*.c)
$(wildcard Source/*/*/*/*.c)
endef

SRC = $(FIND_C_FILES)
OBJ = $(addprefix $(OBJ_OUTPUT)/, $(notdir $(SRC:.c=.o)))
JSON = $(addprefix $(OBJ_OUTPUT)/, $(notdir $(OBJ:.o=.o.json)))

all: $(GAME)

install:
	@echo 'Starting Build => Compiler:$(CC) | Platform:$(PLATFORM) | Config:$(CONFIG)'
	@mkdir -p $(BIN_OUTPUT)
	@mkdir -p $(OBJ_OUTPUT)
	@mkdir -p $(CONFIG_DIR)
	@mkdir -p $(CONTENT_DIR)

clean:
	@echo 'Cleaning Everything'
	@rm -rf $(BUILD_DIR)
	@rm -rf $(INTERMEDIATE_DIR)

run: $(GAME)
	@echo 'Running -> $(BIN_OUTPUT)/$(GAME)$(EXTENSION)'
	@./$(BIN_OUTPUT)/$(GAME)$(EXTENSION)

rebuild: clean run

$(GAME): install $(OBJ)
	@echo 'Linking -> $(BIN_OUTPUT)/$(GAME)$(EXTENSION)'
	@$(CC) $(OBJ) $(FLAGS) $(LIBS_PATH) $(LIBS) -o$(BIN_OUTPUT)/$(GAME)$(EXTENSION)
	@$(CL_GENERATE)
	@$(MAKE_PAKAGE)

$(OBJ_OUTPUT)/%.o: Source/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CL_COMPILER) $< $(FLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_OUTPUT)/%.o: Source/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CL_COMPILER) $< $(FLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_OUTPUT)/%.o: Source/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CL_COMPILER) $< $(FLAGS) $(INCLUDES) $(DEFINES) -c -o$@
$(OBJ_OUTPUT)/%.o: Source/*/*/*/%.c
	@echo 'Compiling >> $<'
	@$(CC) $(CL_COMPILER) $< $(FLAGS) $(INCLUDES) $(DEFINES) -c -o$@

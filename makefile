CC := g++
CFLAGS := -std=c++11 -Wall -g
GLFLAGS := -lglfw -lglew -lglm -framework OpenGL

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

INCLUDE := -Iinclude

EXE := $(BIN_DIR)/modelTransformations
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(EXE)
	$(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(GLFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(INCLUDE) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)

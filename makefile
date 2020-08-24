CXX = g++ 
C++_VERSION = c++17
CXXFLAGS = -std=$(C++_VERSION) -Wall -w -g 

OUT_DIR = bin
LAUNCHER_NAME = Danger
SRC_DIR = src
ENTRY_POINT = $(SRC_DIR)/main.cpp

GLAD_ROOT = outsourced/glad/
GLAD_INC = $(GLAD_ROOT)include
GLAD_SRC = $(GLAD_ROOT)src

GLFW_ROOT = outsourced/glfw-3.3.2/MinGWmingw32/
GLFW_INC = $(GLFW_ROOT)include
GLFW_LIB = $(GLFW_ROOT)lib
GLFW_SRC = $(GLFW_ROOT)../src


SPDLOG_ROOT = outsourced/spdlog/
SPDLOG_INC = $(SPDLOG_ROOT)include
SPDLOG_SRC = $(SPDLOG_ROOT)src

STBIMAGE_ROOT = outsourced/stbimage

GLM_ROOT = outsourced/glm/

INC_INTERNAL = -I $(SRC_DIR) 
INC=-I $(SRC_DIR)/ -I $(GLAD_INC) -I $(GLFW_INC) -I $(GLM_ROOT) -I $(STBIMAGE_ROOT)
LIBS = -L $(GLFW_LIB)
LINKS = -lglfw3 -lglu32 -lopengl32 -lgdi32



OBJECTS = $(OUT_DIR)/DGRCore.o $(OUT_DIR)/FileManip.o $(OUT_DIR)/Context.o $(OUT_DIR)/Shader.o
all: main

main: $(ENTRY_POINT) DGRCore.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(INC) -o $(OUT_DIR)/$(LAUNCHER_NAME) $(OBJECTS) $(ENTRY_POINT) $(GLAD_SRC)/glad.c $(LINKS)


ALL_SETTINGS = $(CXX) $(CXXFLAGS) $(LIBS) $(INC) $(INC_INTERNAL)
DGRCore.o: $(SRC_DIR)/DGRCore.cpp FileManip.o Context.o Shader.o
	$(ALL_SETTINGS) -c $(SRC_DIR)/DGRCore.cpp -o $(OUT_DIR)/DGRCore.o 

FileManip.o: $(SRC_DIR)/util/FileManip.cpp
	$(ALL_SETTINGS) -c $(SRC_DIR)/util/FileManip.cpp -o $(OUT_DIR)/FileManip.o

Context.o: $(SRC_DIR)/Context.cpp
	$(ALL_SETTINGS) -c $(SRC_DIR)/Context.cpp -o $(OUT_DIR)/Context.o

Shader.o: $(SRC_DIR)/Structures/Shader.cpp
	$(ALL_SETTINGS) -c $(SRC_DIR)/Structures/Shader.cpp -o $(OUT_DIR)/Shader.o


run: $(OUT_DIR)/$(LAUNCHER_NAME).exe
	./$(OUT_DIR)/$(LAUNCHER_NAME).exe

launch:
	make
	make run


fetch:
	git fetch --all
	git reset --hard origin/master


clean:
	del $(OUT_DIR)\$(LAUNCHER_NAME).exe
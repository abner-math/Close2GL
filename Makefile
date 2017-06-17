CXX=g++
GLUI_LIBS=-I ./glui -I ./glui/include
INCLUDE_LIBS=$(GLUI_LIBS)
CXX_FLAGS=-std=c++11 $(INCLUDE_LIBS)
OPENGL_LIBS=-lGL -lGLU -lglut
OPENCV_LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
LD_FLAGS=$(OPENGL_LIBS) $(OPENCV_LIBS)

TARGET=Assignment4

SRC_FILES=$(wildcard src/*.cpp)
OBJ_FILES=$(patsubst src/%.cpp,obj/%.o,$(SRC_FILES))
GLUI_OBJ_FILES=$(wildcard glui/*.o)

obj/%.o: src/%.cpp src/%.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@ 
$(TARGET): $(OBJ_FILES) main.cpp
	$(CXX) $(CXX_FLAGS) -o $(TARGET) main.cpp $(OBJ_FILES) $(GLUI_OBJ_FILES) $(LD_FLAGS)
default: $(TARGET)
clean:
	rm -rf $(TARGET) obj/* 
all: clean default

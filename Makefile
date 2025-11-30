SRC_DIR := .
OBJ_DIR := .

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CXX      := g++
CXXFLAGS := -g -Wall -fpermissive -std=c++17
LDFLAGS  := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

TARGET := mandelbrot.out

$(TARGET): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ_FILES)
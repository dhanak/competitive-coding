#CXX=clang++ -Xclang -flto-visibility-public-std
CXXFLAGS=-Wall -Wpedantic -Werror -std=c++14 -isystem "/cygdrive/c/program files/boost/boost_1_67_0" -O2 #-g
BUILD_DIR=build
TARGET=emd.exe opencv_solver.exe

CPP=$(wildcard *.cpp)
OBJ=$(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP=$(OBJ:%.o=%.d)

all build: $(TARGET)

emd.exe: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

opencv_solver.exe: opencv_solver.cc
	$(CXX) $^ $(CXXFLAGS) $(shell pkg-config opencv --cflags --libs) -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

test: build
	time ./$(TARGET) <2.in

clean:
	-$(RM) -rf $(TARGET) $(BUILD_DIR)

.PHONY: all build test clean

-include $(DEP)

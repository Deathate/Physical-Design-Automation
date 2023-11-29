CXX = g++
SRC = main.cpp
CXXFLAGS = -std=c++23
TARGET = Floorplan
TARGET_TEST = main

all: $(TARGET)
.PHONY: $(TARGET_TEST) # make sure always run the target
test: $(TARGET_TEST)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -O2 -o $(TARGET)

$(TARGET_TEST): $(SRC)
	$(CXX) $(SRC) -std=c++23 -I "../.." -g -O2 -o $(TARGET_TEST)
clean:
	rm -f $(TARGET)
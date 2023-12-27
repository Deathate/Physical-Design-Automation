CXX = g++
SRC = main.cpp
CXXFLAGS = -std=c++20
TARGET = Lab3
TARGET_TEST = main

all: $(TARGET)
.PHONY: $(TARGET_TEST) # make sure always run the target
test: $(TARGET_TEST)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -L /usr/lib -O2 -o $(TARGET)

$(TARGET_TEST): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -DDEBUG -I "../.." -O2 -o $(TARGET_TEST)
clean:
	rm -f $(TARGET)
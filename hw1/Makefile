CXX = g++
CXXFLAGS = -std=c++20
TARGET = Lab1
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -I '../..' $(SRC) -O2 -o $(TARGET)

clean:
	rm -f $(TARGET)
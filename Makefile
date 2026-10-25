CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -MMD -MP

TARGET := cal++
SOURCES := main.cc Date.cc
OBJECTS := $(SOURCES:.cc=.o)
DEPENDS := $(OBJECTS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPENDS)

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDS)

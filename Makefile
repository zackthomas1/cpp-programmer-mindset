CXX=g++
CXXFLAGS=-ggdb -std=c++20 -Wall -Wextra -Werror -Wfatal-errors -pedantic

BINDIR=bin
OBJDIR=obj

TARGET=$(BINDIR)/main.exe
SRC=src/01_thinkingcomputationally/main.cpp

ifeq ($(OS),Windows_NT)
RM=del /Q
else
RM=rm -f
endif

main: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean: 
	$(RM) $(TARGET)
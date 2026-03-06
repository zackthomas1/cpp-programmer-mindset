CXX=g++
CXXFLAGS=-ggdb -std=c++17 -Wall -Wextra -Werror -Wfatal-errors -pedantic

BINDIR=..\..\bin
OBJDIR=..\..\obj

TARGET=$(BINDIR)\main.exe
SRC=main.cpp

ifeq ($(OS),Windows_NT)
RM=del /Q
else
RM=rm -f
endif

main: main.cpp 
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean: 
	$(RM) $(TARGET)
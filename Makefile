CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude
LDFLAGS =

SRCDIR = src
INCDIR = include
BINDIR = .

SRC_FILES_UTILS = $(SRCDIR)/Recipe.cpp \
                  $(SRCDIR)/Date.cpp \
                  $(SRCDIR)/MealPlan.cpp \
                  $(SRCDIR)/RecipePlannerUtils.cpp

SRC_FILES_APPS = $(SRCDIR)/app_1.cpp \
                 $(SRCDIR)/app_2.cpp

OBJ_FILES_UTILS = $(patsubst $(SRCDIR)/%.cpp, obj/%.o, $(SRC_FILES_UTILS))
OBJ_FILES_APPS = $(patsubst $(SRCDIR)/%.cpp, obj/%.o, $(SRC_FILES_APPS))

# Command to ensure the obj directory exists.
MAKE_DIR_CMD = cmd /c "if not exist obj mkdir obj"

all: $(BINDIR)/app_1 $(BINDIR)/app_2

$(BINDIR)/app_1: obj/app_1.o $(OBJ_FILES_UTILS)
	$(MAKE_DIR_CMD)
	$(CXX) $(CXXFLAGS) obj/app_1.o $(OBJ_FILES_UTILS) -o $@ $(LDFLAGS)

$(BINDIR)/app_2: obj/app_2.o $(OBJ_FILES_UTILS)
	$(MAKE_DIR_CMD)
	$(CXX) $(CXXFLAGS) obj/app_2.o $(OBJ_FILES_UTILS) -o $@ $(LDFLAGS)

obj/%.o: $(SRCDIR)/%.cpp
	$(MAKE_DIR_CMD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo Cleaning up...
	-del /q $(BINDIR)\app_1 $(BINDIR)\app_2 2>nul
	-del /q obj\*.o 2>nul
	-rmdir /s /q obj 2>nul
	@echo Clean complete.

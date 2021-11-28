PATH    = ../mingw32/bin
CXX     = "../mingw32/bin/g++.exe"
SRC     = base/*.cpp scene/*.cpp ai/*.cpp
OUT     = Project
DEBUG   = compile.txt

SFMLDIR = ../SFML-2.5.1
LINKER  = -I $(SFMLDIR)/include -L $(SFMLDIR)/lib
FLAGS   = -static -DSFML_STATIC
LIBS    = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32

.PHONY: clean 
make: clean
	$(CXX) $(SRC) -o $(OUT).exe $(FLAGS) $(LINKER) $(LIBS) 2>$(DEBUG)
	@if exist $(OUT).exe (echo Compilation successful.)
clean:
	@if exist $(OUT).exe (del $(OUT).exe)

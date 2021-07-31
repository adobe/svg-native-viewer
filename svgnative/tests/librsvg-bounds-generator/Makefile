LIBRSVG_INCLUDES = $(shell pkg-config librsvg-2.0 --cflags)
LIBRSVG_LIBS = $(shell pkg-config librsvg-2.0 --libs)
all: mk_folder
	g++ -ggdb -O0 script.cpp -o build/script $(LIBRSVG_INCLUDES) $(LIBRSVG_LIBS)
mk_folder:
	mkdir -p build


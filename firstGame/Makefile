EXE=first.exe
SRC=first.cpp
CFLAGS=-I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LIBS=-L/opt/homebrew/lib -lSDL2
RPATH=-Wl,-rpath,/opt/homebrew/lib

${EXE}: ${SRC}
	g++ ${CFLAGS} $< -o $@ ${LIBS} ${RPATH}
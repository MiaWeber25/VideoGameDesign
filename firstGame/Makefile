EXE=first.exe
SRC=first.cpp
CFLAGS=-I/Library/Frameworks/SDL2.framework/Headers -D_THREAD_SAFE
# -I/usr/local/include/SDL2
#LIBS=-lSDL2

${EXE}:${SRC}
	g++ ${CFLAGS} $< -o $@ -F/Library/Frameworks/ -framework SDL2
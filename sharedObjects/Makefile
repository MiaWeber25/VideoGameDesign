EXE=sample.exe
SRC=sample.cpp
LIB=fun
DLL=lib${LIB}.so
DLLSRC=fun.c
LDFLAGS= -l ${LIB} -L . 
# Loading a library (indicated with -l) in this case the current library .
# sudo cp libfun.so /usr/lib
# LD_PRELOAD=libfun.so ./sample.exe

${EXE}: ${SRC}
	g++ $< ${DLL} -o $@ ${LDFLAGS}

# Make a shared object (DLL) = compile with -shared option
# Static loading (better for small games like this) and dynamically loading a DLL (dynamic linked library)
${DLL}:${DLLSRC}
	gcc -shared $< -o $@

clean:
	rm fun.o fun.so libfun.so sample.exe

cleanSO:
	rm libfun.so
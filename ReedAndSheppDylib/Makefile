all : 32 64
  
32 :
	clang -arch i386 -shared -undefined dynamic_lookup ReedAndSheppDylib.c -o ReedAndSheppDylib32.dylib 

64 :
	clang -arch x86_64 -shared -undefined dynamic_lookup ReedAndSheppDylib.c -o ReedAndSheppDylib64.dylib 

ReedAndSheppDylib64 : RSD.o
	cc -arch x86_64 -g -dynamiclib -Wl,-headerpad_max_install_names -o TESTReedAndSheppDylib64.dylib  -install_name TESTReedAndSheppDylib64.dylib RSD.o

RSD.o :
	cc -Dmylib_EXPORTS -arch x86_64 -g -fPIC -o RSD.o -c ReedAndSheppDylib.cpp

clean :
	rm ReedAndSheppDylib32.dylib ReedAndSheppDylib64.dylib 
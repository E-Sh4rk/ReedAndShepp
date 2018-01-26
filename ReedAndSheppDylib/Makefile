all : 32 64
  
32 :
	clang -arch i386 -shared -undefined dynamic_lookup ReedAndSheppDylib.c -o ReedAndSheppDylib32.dylib 

64 :
	clang -arch x86_64 -shared -undefined dynamic_lookup ReedAndSheppDylib.c -o ReedAndSheppDylib64.dylib 

Test32 : RSD32
	cc -arch i386 -g -dynamiclib -Wl,-headerpad_max_install_names -o TESTReedAndSheppDylib32.dylib -install_name TESTReedAndSheppDylib32.dylib RSD.o

Test64 : RSD64
	cc -arch x86_64 -g -dynamiclib -Wl,-headerpad_max_install_names -o TESTReedAndSheppDylib64.dylib -install_name TESTReedAndSheppDylib64.dylib RSD.o

RSD32 :
	cc -Dmylib_EXPORTS -arch i386 -g -fPIC -o RSD.o -c ReedAndSheppDylib.c

RSD64 :
	cc -Dmylib_EXPORTS -arch x86_64 -g -fPIC -o RSD.o -c ReedAndSheppDylib.c

clean :
	rm ReedAndSheppDylib32.dylib ReedAndSheppDylib64.dylib 
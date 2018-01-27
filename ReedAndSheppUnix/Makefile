all : linux

linux : linux32 linux64

mac : mac32 mac64
  
mac32 :
	clang -arch i386 -shared -undefined dynamic_lookup ReedAndShepp.c -o ReedAndShepp.dylib

mac64 :
	clang -arch x86_64 -shared -undefined dynamic_lookup ReedAndShepp.c -o ReedAndShepp64.dylib

linux32 :
	clang -arch i386 -shared -undefined dynamic_lookup -fPIC ReedAndShepp.c -o ReedAndShepp.so

linux64 :
	clang -arch x86_64 -shared -undefined dynamic_lookup -fPIC ReedAndShepp.c -o ReedAndShepp64.so

clean :
	rm ReedAndShepp.dylib ReedAndShepp64.dylib ReedAndShepp.so ReedAndShepp64.so 

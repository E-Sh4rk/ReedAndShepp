all : 32 64
  
32 :
	clang -arch i386 -dynamiclib ReedAndSheppDylib.cpp -fvisibility=hidden -o ReedAndSheppDylib32.dylib 

64 :
	clang -arch x86_64 -dynamiclib ReedAndSheppDylib.cpp -fvisibility=hidden -o ReedAndSheppDylib64.dylib 

clean :
	rm ReedAndSheppDylib32.dylib ReedAndSheppDylib64.dylib 
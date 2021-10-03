all: bin/cppshot.exe

bin/cppshot.exe: obj/main.o obj/resources.res
	g++ -o bin/cppshot.exe obj/main.o  obj/resources.res -unicode -lgdiplus  -lgdi32 -luser32 -lkernel32 -lcomctl32

obj/main.o: main.cpp resources.h
	g++ -Wall -g -std=c++11  -DUNICODE=1 -c main.cpp -o obj/main.o

obj/resources.res: resources.rc
	windres -J rc -O coff -i resources.rc -o obj/resources.res
main.out: main.o lib/libarm.a lib/libbox2d.a lib/libnn.a gauge.o
	g++ main.o gauge.o -o main.out -Llib/ -larm -lbox2d -lnn -lsfml-graphics -lsfml-window -lsfml-system -pthread

gauge.o: src/gauge.cpp include/gauge.h
	g++ -c src/gauge.cpp

main.o: main.cpp
	g++ -c main.cpp

main.out: main.o lib/libarm.a lib/libbox2d.a lib/libnn.a
	g++ main.o -o main.out -Llib/ -larm -lbox2d -lnn -lsfml-graphics -lsfml-window -lsfml-system -pthread

main.o: main.cpp
	g++ -c main.cpp

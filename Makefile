main.out: main.o lib/arm.o lib/libbox2d.a lib/libnn.a
	g++ main.o lib/arm.o -o main.out -Llib/ -lbox2d -lnn -lsfml-graphics -lsfml-window -lsfml-system -pthread

main.o: main.cpp
	g++ -c main.cpp

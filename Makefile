CC = g++
STD = -std=c++2a
CFLAGS = $(STD) -g -Wall # -pg
O = main.o ChaosMagic.o MoveMake.o

cc0: $(O)
	$(CC) $(CFLAGS) -o $@ $(O)

main.o: main.cpp ChaosMagic.h Board.h MoveMake.h
	$(CC) $(CFLAGS) -c main.cpp

ChaosMagic.o: ChaosMagic.cpp ChaosMagic.h
	$(CC) $(CFLAGS) -c ChaosMagic.cpp

MoveMake.o: MoveMake.cpp MoveMake.h ChaosMagic.h Board.h Move.h
	$(CC) $(CFLAGS) -c MoveMake.cpp

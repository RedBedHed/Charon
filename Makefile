CC = clang++
STD = -std=c++2a
CFLAGS = $(STD) -Ofast -pipe -march=native -Wall -flto -DNDEBUG -mpopcnt -m64 -mbmi2 # -pg
O = main.o ChaosMagic.o MoveMake.o

cc0: $(O)
	$(CC) $(CFLAGS) -o $@ $(O)

main.o: main.cpp ChaosMagic.h Board.h MoveMake.h
	$(CC) $(CFLAGS) -c main.cpp

ChaosMagic.o: ChaosMagic.cpp ChaosMagic.h
	$(CC) $(CFLAGS) -c ChaosMagic.cpp

MoveMake.o: MoveMake.cpp MoveMake.h ChaosMagic.h Board.h Move.h
	$(CC) $(CFLAGS) -c MoveMake.cpp

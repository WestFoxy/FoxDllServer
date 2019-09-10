OBJS	= AccountsManager.o DLLManager.o FoxConnect.o Log.o main.o
SOURCE	= AccountsManager.cpp DLLManager.cpp FoxConnect.cpp Log.cpp main.cpp
HEADER	= AccountsManager.h DLLManager.h FoxConnect.h includes.h json.hpp Log.h
OUT	= FoxyServer
CC	 = g++
FLAGS	 = -g -c
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

AccountsManager.o: AccountsManager.cpp
	$(CC) $(FLAGS) AccountsManager.cpp -std=c++17

DLLManager.o: DLLManager.cpp
	$(CC) $(FLAGS) DLLManager.cpp -std=c++17

FoxConnect.o: FoxConnect.cpp
	$(CC) $(FLAGS) FoxConnect.cpp -std=c++17

Log.o: Log.cpp
	$(CC) $(FLAGS) Log.cpp -std=c++17

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -std=c++17


clean:
	rm -f $(OBJS) $(OUT)

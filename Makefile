TARGET=fiboBG
SRC=main.c
# DEBUG=-g
DEBUG=
$(TARGET): main.c config.h
	gcc -Wall -Wextra $(DEBUG) $(SRC) `pkg-config --cflags --libs cairo x11` -o $(TARGET) -lm

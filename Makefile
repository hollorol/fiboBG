TARGET=fiboBG
DEST=$(HOME)/.local/bin/ #local install
#DES=/usr/local #global install
SRC=main.c
# DEBUG=-g
DEBUG=
$(TARGET): main.c config.h
	gcc -Wall -Wextra -pedantic $(DEBUG) $(SRC) `pkg-config --cflags --libs cairo x11` -o $(TARGET) -lm
install:
	cp $(TARGET) $(DEST)
uninstall:
	rm $(DEST)/$(TARGET)

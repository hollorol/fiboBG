TARGET=fiboBG
DEST=$(HOME)/.local/bin
SRC=main.c
# DEBUG=-g
DEBUG=
$(TARGET): main.c config.h
	gcc -Wall -Wextra $(DEBUG) $(SRC) `pkg-config --cflags --libs cairo x11` -o $(TARGET) -lm
install:
	cp $(TARGET) $(DEST)/
uninstall:
	rm $(DEST)/$(TARGET)

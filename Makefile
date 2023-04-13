# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS= -mavx2 -c -O3 -DDRAW

all: mandelbrot execute clean	

mandelbrot: mandelbrot.o
	$(CC) mandelbrot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

mandelbrot.o:
	$(CC) $(CFLAGS) mandelbrot.cpp -o mandelbrot.o

clean:
	rm *.o

execute:
	./sfml-app
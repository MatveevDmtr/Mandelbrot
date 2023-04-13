# Переменная CC указывает компилятор, используемый для сборки
CC=g++
#В переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS= -mavx2 -c -O3 -DDRAW
#Флаг оптимизации
OPTFLAG= -O3

all: mandelbrot execute clean	

mandelbrot: mandelbrot.o
	$(CC) mandelbrot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system

mandelbrot.o:
	$(CC) $(CFLAGS) $(OPTFLAG) mandelbrot.cpp -o mandelbrot.o

clean:
	rm *.o

execute:
	./sfml-app
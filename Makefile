all:
	g++ -c mandelbrot.cpp -o mandelbrot.o
	g++ mandelbrot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./sfml-app

clear:
	rm -f *.o

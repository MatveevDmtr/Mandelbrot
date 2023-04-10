#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>



void DrawMandelbrot(sf::Image *image);
void DrawMandelbrotIntrs(sf::Image *image);
sf::Text *SetText (sf::Font &font, float x_coord, float y_coord);
void ChangeZoom();
__m256i CountInterations(__m256 x_arr, __m256 y_arr);
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <math.h>



void DrawMandelbrot(sf::RenderWindow &window);
void DrawMandelbrotIntrs(sf::Image *image);
sf::Text *SetText (sf::Font &font, float x_coord, float y_coord);

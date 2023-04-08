
#include "mandelbrot.hpp"
#include <immintrin.h>

const int W_HEIGHT = 1000;
const int W_WIDTH = 1000;
const int MAX_ITERATION = 255;
const float MAX_DISTANCE = 100.0;
typedef unsigned char BYTE;

const float dx = 0.004;
const float dy = 0.004;

const int rescale_factor_x = 250;
const int rescale_factor_y = 250;


const float SPACE_X = 2.0;
const float SPACE_Y = 2.0;

float x_start = -SPACE_X;
float x_finish = SPACE_X;

float y_start = -SPACE_Y;
float y_finish = SPACE_Y;


int main() 
{
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "Witcherok");
    window.setFramerateLimit(30);

    DrawMandelbrot(window);
    window.display();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                {
                    printf("Left\n");
                    x_start  += 5 * dx;
                    x_finish += 5 * dx;
                    
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    printf("Right\n");
                    x_start  -= 5 * dx;
                    x_finish -= 5 * dx;
                }

                if (event.key.code == sf::Keyboard::Down)
                {
                    printf("Down\n");
                    y_start  -= 5 * dx;
                    y_finish -= 5 * dx;
                }

                if (event.key.code == sf::Keyboard::Up)
                {
                    printf("Up\n");
                    y_start  += 5 * dx;
                    y_finish += 5 * dx;
                }

                DrawMandelbrot(window);
                window.display();
            }        
        }
        
        
    }

    return 0;    
}


void DrawMandelbrot(sf::RenderWindow &window) 
{
    sf::RectangleShape Pixel(sf::Vector2f(1, 1));

    for (float y0 = y_start; y0 <= y_finish; y0 += dy) 
    {
        float y0_pos = (-y_start - y0) * rescale_factor_y;
        
        for (float x0 = x_start; x0 <= x_finish; x0 += dx)
        {
            float x0_pos = (-x_start + x0) * rescale_factor_x;
            
            float x = x0, y = y0;

            int cur_iter = 0;

            while (cur_iter < MAX_ITERATION) 
            {
                float x2 = x * x, y2 = y * y, xy = x * y;
                float distance = x2 + y2;

                if (distance > MAX_DISTANCE)    break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

                cur_iter++;
            }

            Pixel.setPosition(x0_pos, y0_pos);
            Pixel.setFillColor(sf::Color{(BYTE) cur_iter, 0, (BYTE) 255 - cur_iter});
            window.draw(Pixel);
        }
    }
}
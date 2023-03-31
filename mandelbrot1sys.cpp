
#include "mandelbrot.hpp"

const int W_HEIGHT = 1000;
const int W_WIDTH = 1000;
const int SPACE_X = (int) (W_WIDTH / 2);
const int SPACE_Y = (int) (W_HEIGHT / 2);
const int MAX_ITERATION = 255;
const long long int MAX_DISTANCE = 1000000000000000;
typedef unsigned char BYTE;

const int dx = 4;
const int dy = 4;

int x_start = -500;
int x_finish = 500;

int y_start = -500;
int y_finish = 500;


int main() 
{
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "Witcherok");
    window.setFramerateLimit(30);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            //if (event.type == Event::KeyPressed)
            //    if (event.key.code == Keyboard::Left)
        }
        DrawMandelbrot(window);

        window.display();
    }

    return 0;    
}


void DrawMandelbrot(sf::RenderWindow &window) 
{
    sf::RectangleShape Pixel(sf::Vector2f(1, 1));

    for (float y0 = y_start; y0 <= y_finish; y0 += dy) 
    {
        float y0_pos = SPACE_Y - y0;
        
        for (float x0 = x_start; x0 <= x_finish; x0 += dx)
        {
            float x0_pos = SPACE_X + x0;
            
            long long int x = x0, y = y0;

            int cur_iter = 0;

            while (cur_iter < MAX_ITERATION) 
            {
                long long x2 = x * x, y2 = y * y, xy = x * y;
                long long distance = x2 + y2;
                
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
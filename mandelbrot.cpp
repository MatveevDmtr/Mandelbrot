#include "mandelbrot.hpp"
#include <immintrin.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <math.h>

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

float x_start = (-SPACE_X - 0.5);
float x_finish = (SPACE_X - 0.5);

float y_start = -SPACE_Y;
float y_finish = SPACE_Y;


int main() 
{
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "Witcherok");
    window.setFramerateLimit(30);

    //DrawMandelbrotIntrs(window);
    //window.display();

    sf::Clock clock;
    sf::Font font;
    font.loadFromFile("caviar-dreams.ttf");
    sf::Text fps_text = *SetText (font, 5, 5);

    sf::Time spent_time = clock.getElapsedTime();

    sf::Image image;
    image.create(W_WIDTH, W_HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    char buf_text[20] = {};

    while (window.isOpen())
    {
        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();         
        }   

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            //printf("Left\n");
            x_start  += 5 * dx;
            x_finish += 5 * dx;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            //printf("Right\n");
            x_start  -= 5 * dx;
            x_finish -= 5 * dx;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            //printf("Down\n");
            y_start  -= 5 * dy;
            y_finish -= 5 * dy;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            //printf("Up\n");
            y_start  += 5 * dy;
            y_finish += 5 * dy;
        }

        DrawMandelbrotIntrs(&image);
        
        spent_time = clock.getElapsedTime();
        //printf("elapsed time: %d\n", spent_time.asSeconds());
        sprintf (buf_text, "fps: %.2f\n", 1/spent_time.asSeconds());

        window.clear(sf::Color::Black);

        texture.update(image);

        fps_text.setString (buf_text);
        window.draw (sprite);
        window.draw(fps_text);
        window.display();

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

void DrawMandelbrotIntrs(sf::Image *image) 
{
    //printf("%d\n\n\n\n\n\n\n\n", image.getSize().y);
    sf::RectangleShape Pixel(sf::Vector2f(1, 1));

    //__m256 dx_arr   = _mm256_set_ps  (7*dx, 6*dx, 5*dx, 4*dx, 3*dx, 2*dx, dx, 0);
    //__m256i dx_w_arr = _mm256_set_epi32  (7, 6, 5, 4, 3, 2, 1, 0);
    __m256 dx_arr   = _mm256_set_ps  (0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);
    __m256i dx_w_arr = _mm256_set_epi32  (0, 1, 2, 3, 4, 5, 6, 7);
    //__m256 dx_arr   = _mm256_set_ps  (0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);
    __m256 max_dist = _mm256_set1_ps (MAX_DISTANCE);

    __m256 x_pos_shift_arr = _mm256_set1_ps (x_start);
    __m256 y_pos_shift_arr = _mm256_set1_ps (y_start);

    __m256 rescale_factor_x_arr = _mm256_set1_ps (rescale_factor_x);
    __m256 rescale_factor_y_arr = _mm256_set1_ps (rescale_factor_y);

    for (int y0 = 0; y0 < W_HEIGHT; y0++) 
    {
        __m256 y0_arr = _mm256_add_ps(y_pos_shift_arr, _mm256_set1_ps((float)y0 / rescale_factor_y));
        __m256i y0_pos_arr = _mm256_set1_epi32(y0);
        
        for (int x0 = 0; x0 < W_WIDTH; x0 += 8)
        {   
            __m256 x0_arr = _mm256_sub_ps(x_pos_shift_arr, _mm256_set1_ps(-(float)x0 / rescale_factor_x));
            x0_arr = _mm256_add_ps(x0_arr, dx_arr);

            __m256i x0_pos_arr = _mm256_add_epi32(dx_w_arr, _mm256_set1_epi32(x0));

            __m256 x_arr = x0_arr;
            __m256 y_arr = y0_arr;

            __m256i curr_iters_arr = _mm256_set1_epi32(0);

            int curr_iter = 0;

            for( ; curr_iter < MAX_ITERATION; curr_iter++) 
            {
                __m256 x2 = _mm256_mul_ps(x_arr, x_arr);
                __m256 y2 = _mm256_mul_ps(y_arr, y_arr);
                __m256 xy = _mm256_mul_ps(x_arr, y_arr);

                __m256 dist = _mm256_add_ps(x2, y2);

                __m256 cmp_mask = _mm256_cmp_ps(dist, max_dist, _CMP_LT_OQ);

                int res = _mm256_movemask_ps(cmp_mask);                         // translate mask to int
                if (!res)  
                {
                    //printf("res break\n");
                    break;
                }

                //int* mask_arr = (int*) &cmp_mask;
                //printf("cmp_mask: %d %d %d %d\n", mask_arr[0], mask_arr[1], mask_arr[2], mask_arr[3]);

                x_arr = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_arr);           
                y_arr = _mm256_add_ps(_mm256_add_ps(xy, xy), y0_arr);

                curr_iters_arr = _mm256_sub_epi32 (curr_iters_arr, _mm256_castps_si256 (cmp_mask));       // sub because of (-1)s in mask
            }

            //float* x0_8_pos = (float*) &x0_pos_arr;
            //float y0_pos = *((float*) &y0_pos_arr);

            //u_int32_t* x0_8_pos = (u_int32_t*) &x0_pos_arr;
            //u_int32_t  y0_pos  = * (u_int32_t*) &y0_pos_arr;

            int32_t* x0_8_pos = (int32_t*) &x0_pos_arr;
            int32_t  y0_pos  = * (int32_t*) &y0_pos_arr;

            int* curr_iters_int = (int*) &curr_iters_arr;
            //printf("curr_iters_int: %f %f %f %f %f %f %f %f\n", curr_iters_int[0], curr_iters_int[1], curr_iters_int[2], curr_iters_int[3], 
             //                                                   curr_iters_int[4], curr_iters_int[5], curr_iters_int[6], curr_iters_int[7]);

            sf::Color color;

            for (int i = 0; i < 8; i++)
            {
                //Pixel.setPosition(x0_8_pos[i], y0_pos);
                int n = (int) curr_iters_int[i];
                //if (n != -1)
                //    printf("n: %f\n", n);
                //Pixel.setFillColor(sf::Color((BYTE)n * 35, (BYTE) 100 - 2 * n, (BYTE) n * 7));
                //Pixel.setFillColor(sf::Color((BYTE)n * 5, (BYTE) n * 30, (BYTE) 255 - n));

                color = sf::Color((BYTE)n * 35, (BYTE) 100 - 2 * n, (BYTE) n * 7);
                image->setPixel(x0_8_pos[i], y0_pos, color);
                //window.draw(Pixel);
            }
        }
    }
}

sf::Text* SetText (sf::Font &font, float x_coord, float y_coord) 
{
    sf::Text *text = new sf::Text;          

    text->setFont(font);
    text->setFillColor(sf::Color::Yellow);
    text->setCharacterSize(30);
    text->setPosition(x_coord, y_coord);

    return text;
}
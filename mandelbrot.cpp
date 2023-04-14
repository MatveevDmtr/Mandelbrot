#include "mandelbrot.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <math.h>

//#define MEASURING

const int W_HEIGHT        = 1000;
const int W_WIDTH         = 1000;
const int MAX_ITERATION   = 255;
const float MAX_DISTANCE  = 100.0;
const __m256 MAX_DIST_ARR = _mm256_set1_ps (MAX_DISTANCE);

typedef unsigned char BYTE;

float dx = 0.004;
float dy = 0.004;

float rescale_factor_x = 250;
float rescale_factor_y = 250;

const float ZOOM_FACTOR = 1.2;
float curr_zoom_factor = ZOOM_FACTOR;

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

    sf::Clock clock;

    #ifdef DRAW
    sf::Font font;
    font.loadFromFile("fonts/caviar-dreams.ttf");
    sf::Text fps_text = *SetText (font, 5, 5);
    #endif

    sf::Time spent_time_avx   = clock.getElapsedTime();
    sf::Time spent_time_noavx = clock.getElapsedTime();
    
    sf::Image image;
    #ifdef DRAW
    image.create(W_WIDTH, W_HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    char buf_text[20] = {};

    #endif

    int count_measures = 0;
    float time_sum_avx = 0;
    float time_sum_noavx = 0;

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

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F10))     // zoom +
        {
            curr_zoom_factor = 1 / ZOOM_FACTOR;
            ChangeZoom();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))        // zoom -
        {
            curr_zoom_factor = ZOOM_FACTOR;
            ChangeZoom();        
        }

            clock.restart();
        //#if AVX
            DrawMandelbrotIntrs(&image);
            spent_time_avx = clock.getElapsedTime();
        //#else
        #ifdef MEASURING
            clock.restart();
            DrawMandelbrot(&image);
        //#endif
            spent_time_noavx = clock.getElapsedTime();

        if (count_measures < 200)
        {
            time_sum_avx += 1/spent_time_avx.asSeconds();
            time_sum_noavx += 1/spent_time_noavx.asSeconds();
            count_measures++;
        }
        else if (count_measures == 200)
        {
            printf("end of measuring\n");
            count_measures++;
        }
        #endif

        #ifdef DRAW
        sprintf (buf_text, "fps: %.2f\n", 1/spent_time_avx.asSeconds());

        window.clear(sf::Color::Black);

        texture.update(image);

        fps_text.setString (buf_text);

        window.draw (sprite);
        window.draw(fps_text);
        window.display();
        #endif

    }

    #ifdef MEASURING
    printf("Averaged AVX FPS: %.2f\n", time_sum_avx/200);
    printf("Averaged No AVX FPS: %.2f\n", time_sum_noavx/200);
    #endif

    return 0;    
}


void DrawMandelbrot(sf::Image *image) 
{
    sf::Color color;

    int n = 0;

    for (int y0 = 0; y0 < W_HEIGHT; y0++) 
    {
        int y0_pos = y0;
        float y0_phys = y_start + ((float)y0 / rescale_factor_y);
        
        for (int x0 = 0; x0 < W_WIDTH; x0++)
        {
            int x0_pos = x0;
            float x0_phys = x_start + ((float)x0 / rescale_factor_x);
            
            float x = x0_phys, y = y0_phys;

            int cur_iter = 0;

            while (cur_iter < MAX_ITERATION) 
            {
                float x2 = x * x, y2 = y * y, xy = x * y;
                float distance = x2 + y2;

                if (distance > MAX_DISTANCE)    break;

                x = x2 - y2 + x0_phys;
                y = xy + xy + y0_phys;

                cur_iter++;
            }

            n = cur_iter;
            color = sf::Color((BYTE)n * 35, (BYTE) 100 - 2 * n, (BYTE) n * 7);

            #ifdef DRAW
                image->setPixel(x0_pos, y0_pos, color);
            #endif
        }
    }
}

void DrawMandelbrotIntrs(sf::Image *image) 
{
    volatile __m256 dx_arr   = _mm256_set_ps  (0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);
    volatile __m256i dx_w_arr = _mm256_set_epi32  (0, 1, 2, 3, 4, 5, 6, 7);

    volatile __m256 x_pos_shift_arr = _mm256_set1_ps (x_start);
    volatile __m256 y_pos_shift_arr = _mm256_set1_ps (y_start);

    volatile  __m256 rescale_factor_x_arr = _mm256_set1_ps (rescale_factor_x);
    volatile __m256 rescale_factor_y_arr = _mm256_set1_ps (rescale_factor_y);

    for (int y0 = 0; y0 < W_HEIGHT; y0++) 
    {
        __m256 y0_arr = _mm256_add_ps(y_pos_shift_arr, _mm256_set1_ps((float)y0 / rescale_factor_y));
        __m256i y0_pos_arr = _mm256_set1_epi32(y0);
        
        for (int x0 = 0; x0 < W_WIDTH; x0 += 8)
        {   
            volatile __m256 x0_arr = _mm256_add_ps(x_pos_shift_arr, _mm256_set1_ps((float)x0 / rescale_factor_x));
            x0_arr = _mm256_add_ps(x0_arr, dx_arr);

            volatile __m256i x0_pos_arr = _mm256_add_epi32(dx_w_arr, _mm256_set1_epi32(x0));

           volatile __m256i curr_iters_arr = CountInterations(x0_arr, y0_arr);      // count colors for 8 points

            #ifdef DRAW

            volatile int32_t* x0_8_pos = (int32_t*) &x0_pos_arr;
            volatile int32_t  y0_pos  = * (int32_t*) &y0_pos_arr;

            volatile int* curr_iters_int = (int*) &curr_iters_arr;
            sf::Color color;

            for (int i = 0; i < 8; i++)
            {
               volatile int n = (int) curr_iters_int[i];

                color = sf::Color((BYTE)n * 35, (BYTE) 100 - 2 * n, (BYTE) n * 7);
                image->setPixel(x0_8_pos[i], y0_pos, color);
            }

            #endif
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

void ChangeZoom()
{
    float x_range = (x_finish - x_start) / curr_zoom_factor;
    float x_mid   = (x_finish + x_start) / 2;
    x_start       = x_mid - x_range / 2;
    x_finish      = x_mid + x_range / 2;
    float y_range = (y_finish - y_start) / curr_zoom_factor;
    float y_mid   = (y_finish + y_start) / 2;
    y_start       = y_mid - y_range / 2;
    y_finish      = y_mid + y_range / 2;

    dx /= curr_zoom_factor;
    dy /= curr_zoom_factor;
    rescale_factor_x *= curr_zoom_factor;
    rescale_factor_y *= curr_zoom_factor;
}

__m256i CountInterations(__m256 x_arr, __m256 y_arr)
{
    __m256 x0_arr = x_arr;
    __m256 y0_arr = y_arr;

    __m256i curr_iters_arr = _mm256_set1_epi32(0);

    int curr_iter = 0;

    for( ; curr_iter < MAX_ITERATION; curr_iter++) 
    {
        __m256 x2 = _mm256_mul_ps(x_arr, x_arr);
        __m256 y2 = _mm256_mul_ps(y_arr, y_arr);
        __m256 xy = _mm256_mul_ps(x_arr, y_arr);

        __m256 dist = _mm256_add_ps(x2, y2);

        __m256 cmp_mask = _mm256_cmp_ps(dist, MAX_DIST_ARR, _CMP_LT_OQ);

        int res = _mm256_movemask_ps(cmp_mask);                         // translate mask to int
        if (!res) break;

        x_arr = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_arr);           
        y_arr = _mm256_add_ps(_mm256_add_ps(xy, xy), y0_arr);

        curr_iters_arr = _mm256_sub_epi32 (curr_iters_arr, _mm256_castps_si256 (cmp_mask));       // sub because of (-1)s in mask
    }

    return curr_iters_arr;
}
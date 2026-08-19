/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2020/04        luwei        BLE Controller w/o RTOS.
 * 2020/05/04     luwei        BLE Controller based on Contiki.
 * 2020/05/05     luwei        BLE Host+Controller based on Contiki.
 * 2020/05/07     luwei        BLE Controller based on AliOS.
 * 2020/05/09     luwei        BLE Host+Controller based on AliOS.
 * 2020/07/10     luwei        BLE Host+Controller based on AliOS+ESP8266.
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>

static float cosf_my(double sita, int n)
{
    int i;
    double sum=1, t=1;
    //sita = sita * 3.1415926 / 180;
    for (i=1; i<=n; i++) {
        t = t * (-1) * sita * sita / (2*i*(2*i-1));
        sum += t;
    }
    return sum;
}
static double sinf_my(double sita, int n)
{
    int i;
    double sum=sita, t=sita;
    //sita = sita * 3.1415926 / 180;
    for (i=1; i<=n; i++) {
        t = t * (-1) * sita * sita / (2*i*(2*i+1));
        sum += t;
    }
    return sum;
}
/*[0,90] degree*/
static const float sin_tbl[91] = {
    0,
    0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528, 0.121869, 0.139173, 0.156434, 0.173648, 0.190809, 0.207912, 0.224951, 0.241922, 0.258819, 0.275637, 0.292372, 0.309017, 0.325568, 0.342020, 0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.453990, 0.469472, 0.484810, 0.500000, 0.515038, 0.529919, 0.544639, 0.559193, 0.573576, 0.587785, 0.601815, 0.615661, 0.629320, 0.642788, 0.656059, 0.669131, 0.681998, 0.694658, 0.707107, 0.719340, 0.731354, 0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798636, 0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.874620, 0.882948, 0.891007, 0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519, 0.951057, 0.956305, 0.961262, 0.965926, 0.970296, 0.974370, 0.978148, 0.981627, 0.984808, 0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.998630, 0.999391, 0.999848,
    1
};
static float sinf_tab(int degree)
{
    float sin;
    if ((degree / 90) % 2) sin = sin_tbl[90 - (degree % 90)];
    else                   sin = sin_tbl[degree % 90];
    if (degree > 180) sin = -sin;
    return sin;
}

/**
 * @brief Mouse is moving in circle step by step.
 * @note  sin/cos, lookup table in float, Bresenham's algorithm in bcircle.pdf.
 *
 * @param [in] R           - Radius of circle.
 *        [out] p_delta_x  - The pointer to relative move delta in x-coordinate.
 *        [out] p_delta_y  - The pointer to relative move delta in y-coordinate.
 * @return None.
 */
void mouse_move_circle_step(int R, int8_t *p_delta_x, int8_t *p_delta_y)
{
    static int absX = 0, absY = 0;
#if 0
    /* float algorithm. */
    static double sita = 0.0;
    int x = (int)(R * cosf_my(sita, 32));
    int y = (int)(R * sinf_my(sita, 32));
    *p_delta_x = x - absX;
    *p_delta_y = y - absY;
    sita += 0.1;
    if (sita >= (3.1415926 * 2.0)) sita = 0.0;
#elif 0
    /* table lookup algorithm. */
    static int degree = 0;
    int x, y;
    /* cos(sita) = sin(sita + 90) */
    x = (int)((float)R * sinf_tab((degree + 90) % 360));
    y = (int)((float)R * sinf_tab(degree));
    *p_delta_x = x - absX;
    *p_delta_y = y - absY;
    degree++;
    degree %= 360;
#elif 0
    /* Bresenham's circle drawing derivation. */
    static int x = 0, y = 0, p;
    if (x < y) {
        /* main loop for the arc of 1/8 circle. */
        x++;
        if (p < 0) {
            p = p + 4 * x + 6;
        } else {
            y--;
            p = p + 4 * (x - y) + 10;
        }
        *p_delta_x = x - absX;
        *p_delta_y = y - absY;
    } else {
        /* initial values: Quadrant I and clockwise. */
        x = 0;
        y = R;
        p = 3 - 2 * R;
    }
#elif 0
    /* Rasterizing Curves. */
    static int x = 0, y = 0, err;
    if (x < 0) {
        /* main loop for the arc of 1/4 circle. */
        int err_cmp;
        err_cmp = err;
        if (err_cmp <= y) {
            /* e_xy + e_y < 0 */
            err += ++y * 2 + 1;
        }
        if (err_cmp > x || err > y) {
            /* e_xy + e_x > 0 or no 2nd y-step */
            err += ++x * 2 + 1;
        }
        *p_delta_x = x - absX;
        *p_delta_y = y - absY;
    } else {
        /* initial Quadrant II and clockwise. */
        x = -R;
        y = 0;
        err = 2 - 2 * R;
    }
#else
    /* bcircle.pdf */
    static int x = 0, y = 0, xc, yc, err, octet = 0;
    /* main loop for the arc of 1/8 circle.
       2*(Re + yc)*xc + xc^2 < 0
       2*(Re + xc)*yc + yc^2 < 0 */
    if (0 == x && 0 == y) {
        /* initial values: Quadrant I0 and counterclockwise. */
        x = R;
        y = 0;
        xc = 1 - 2 * R;
        yc = 1;
        err = 0;
    } else if (0 == octet && x >= y) {
        /* I0: 
           y++;         x-- or x;
           yc = 1+2*yi; xc = 1-2*xi < 0; 
           2*(Re + yc) + xc > 0 */
        y++;
        err += yc;
        yc += 2;
        if (2*err + xc > 0) {
            x--;
            err += xc;
            xc += 2;
        }
        if (x < y) {
            /* Quadrant I1. */
            //err = 0;
            //xc = 1 - 3*R/2; //0.707=sin45=cos45
            //yc = 1 + 3*R/2;
            octet++;
        }
    } else if (1 == octet && x >= 0) {
        /* I1: 
           x--;         y++ or y;
           xc = 1-2*xi; yc = 1+2*yi > 0; 
           2*(Re + xc) + yc < 0 */
        x--;
        err += xc;
        xc += 2;
        if (2*err + yc < 0) {
            y++;
            err += yc;
            yc += 2;
        }
        if (x < 0) {
            /* Quadrant II2. */
            err = 0;
            xc = 1;
            yc = 1 - 2 * R;
            octet++;
            //x = 0;
            //y = R;
        }
    } else if (2 == octet && -x <= y) {
        /* II2:
           x--;         y-- or y;
           xc = 1-2*xi; yc = 1-2*yi < 0;
           2*(Re + xc) + yc > 0 */
        x--;
        err += xc;
        xc += 2;
        if (2*err + yc > 0) {
            y--;
            err += yc;
            yc += 2;
        }
        if (-x > y) {
            /* Quadrant II3. */
            //err = 0;
            //xc = 1 - -3*R/2; //0.707=sin45=cos45
            //yc = 1 - 3*R/2;
            octet++;
        }
    } else if (3 == octet && y >= 0) {
        /* II3:
           y--;         x-- or x;
           yc = 1-2*yi; xc = 1-2*xi > 0;
           2*(Re + yc) + xc < 0 */
        y--;
        err += yc;
        yc += 2;
        if (2*err + xc < 0) {
            x--;
            err += xc;
            xc += 2;
        }
        if (y < 0) {
            /* Quadrant III4. */
            err = 0;
            xc = 1 + 2*-R;
            yc = 1;
            octet++;
            //x = -R;
            //y = 0;
        }
    } else if (4 == octet && -x >= -y) {
        /* III4:
           y--;         x++ or x;
           yc = 1-2*yi; xc = 1+2*xi < 0;
           2*(Re + yc) + xc > 0 */
        y--;
        err += yc;
        yc += 2;
        if (2*err + xc > 0) {
            x++;
            err += xc;
            xc += 2;
        }
        if (-x < -y) {
            /* Quadrant III5. */
            //err = 0;
            //xc = 1 + 3*-R/2; //0.707=sin45=cos45
            //yc = 1 - 3*-R/2;
            octet++;
        }
    } else if (5 == octet && -x >= 0) {
        /* III5: 
           x++;         y-- or y;
           xc = 1+2*xi; yc = 1-2*yi > 0; 
           2*(Re + xc) + yc < 0 */
        x++;
        err += xc;
        xc += 2;
        if (2*err + yc < 0) {
            y--;
            err += yc;
            yc += 2;
        }
        if (-x < 0) {
            /* Quadrant IV6. */
            err = 0;
            xc = 1;
            yc = 1 + 2 * -R;
            octet++;
            //x = 0;
            //y = -R;
        }
    } else if (6 == octet && x <= -y) {
        /* IV6:
           x++;         y++ or y;
           xc = 1+2*xi; yc = 1+2*yi < 0;
           2*(Re + xc) + yc > 0 */
        x++;
        err += xc;
        xc += 2;
        if (2*err + yc > 0) {
            y++;
            err += yc;
            yc += 2;
        }
        if (x > -y) {
            /* Quadrant IV7. */
            //err = 0;
            //xc = 1 + 3*R/2; //0.707=sin45=cos45
            //yc = 1 + 3*-R/2;
            octet++;
        }
    } else if (7 == octet && -y >= 0) {
        /* IV7:
           y++;         x++ or x;
           yc = 1+2*yi; xc = 1+2*xi > 0;
           2*(Re + yc) + xc < 0 */
        y++;
        err += yc;
        yc += 2;
        if (2*err + xc < 0) {
            x++;
            err += xc;
            xc += 2;
        }
        if (-y < 0) {
            /* Quadrant I0. */
            err = 0;
            xc = 1 - 2*R;
            yc = 1;
            octet = 0;
            x = R;
            y = 0;
        }
    }
    *p_delta_x = x - absX;
    *p_delta_y = y - absY;
    //printf("%d: (%d,%d) delta(%d,%d) err=%d xc=%d yc=%d\n", octet, x, y, *p_delta_x, *p_delta_y, err, xc, yc);
#endif

    /* record the track. */
    absX = x;
    absY = y;
}

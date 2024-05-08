/*
 * Author: Xinlong Li
 * Date: 2024.5.6
 * Description: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *
 * lxl_ceiling_effect.c - Ceiling effect module
 */

#include "lxl_ceiling_effect.h"
#include "lxl_param.h"

float ceiling_effect_coefficient(float h_ceiling)
{
    float ce_coefficient;

    // The nominal minmum distance from propeller plane to ceiling plane,
    // if h_ceiling = 0.1R, Then ce_coefficient = 4.37
    if (h_ceiling < 0.25 * PROPELLER_RADIUS)
    {
        // where we let h_ceiling = 0.25R
        ce_coefficient = 1.37;
    }
    else
    {
        ce_coefficient = 0.5 + 0.5 * sqrt(1 + (PROPELLER_RADIUS * PROPELLER_RADIUS) / (8 * h_ceiling * h_ceiling));
    }
    return ce_coefficient;
}

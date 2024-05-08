/*
 * Author: Xinlong Li
 * Date: 2024.5.6
 * Description: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *
 * lxl_ground_effect.c - Ground effect module
 */

#include "lxl_ground_effect.h"
#include "lxl_param.h"

float ground_effect_coefficient(float dist_to_ground)
{
    float ge_coefficient;

    // The nominal minmum distance from propeller plane to ground plane,
    // if h_ground = 0.25R, Then ge_coefficient = infinity
    if (dist_to_ground < 0.5 * PROPELLER_RADIUS)
    {
        ge_coefficient = 2.0;
    }
    else
    {
        ge_coefficient = 1.0 / (1.0 - pow((PROPELLER_RADIUS / (4 * dist_to_ground)), 2));
    }
    return ge_coefficient;
}

/*
 * Author: Xinlong Li
 * Date: 2024.5.6
 * Description: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *
 * lxl_ceiling_effect.c - Ceiling effect module
 */

#include "lxl_ceiling_effect.h"
#include "lxl_param.h"
#include <math.h>

/**
 * @brief Calculate the distance from the propeller plane to the ceiling 
 *         plane，internal used only
 * @param z_position The z-axis position of the quadrotor
 * @return The distance from the propeller plane to the ceiling plane
 */
static float distance_to_ceiling(float z_position)
{
    float dist_to_ceiling = CEILING_HIGHT - z_position;
    return dist_to_ceiling;
}

/**
 * @brief Calculate the ceiling effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ceiling effect coefficient
 */

float ceiling_effect_coefficient(float z_position)
{
    float ce_coefficient;
    float dist_to_ceiling = distance_to_ceiling(z_position);

    // The nominal minmum distance from propeller plane to ceiling plane,
    // if h_ceiling = 0.1R, Then ce_coefficient = 4.37
    if (dist_to_ceiling < 0.25f * PROPELLER_RADIUS)
    {
        // where we let h_ceiling = 0.25R
        ce_coefficient = 1.37;
    }
    else
    {
        ce_coefficient = 0.5f + 0.5f * sqrtf(1.0f + (PROPELLER_RADIUS * PROPELLER_RADIUS) / (8.0f * dist_to_ceiling * dist_to_ceiling));
    }
    return ce_coefficient;
}

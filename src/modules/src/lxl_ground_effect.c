/*
 * Author: Xinlong Li
 * Date: 2024.5.6
 * Description: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *
 * lxl_ground_effect.c - Ground effect module
 */

#include "lxl_ground_effect.h"
#include "lxl_param.h"
#include <math.h>


/**
 * @brief Calculate the distance from the propeller plane to the ground 
 *         plane，internal used only
 * @param z_position The z-axis position of the quadrotor
 * @return The distance from the propeller plane to the ground plane
 */
static float distance_to_ground(float z_position)
{
    float dist_to_ground = z_position;
    return dist_to_ground;
}


/**
 * @brief Calculate the ground effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ground effect coefficient
 */
float ground_effect_coefficient(float z_position)
{
    float ge_coefficient;
    float dist_to_ground = distance_to_ground(z_position);

    // The nominal minmum distance from propeller plane to ground plane,
    // if h_ground = 0.25R, Then ge_coefficient = infinity
    if (dist_to_ground < 0.5f * PROPELLER_RADIUS)
    {
        ge_coefficient = 2.0;
    }
    else
    {
        ge_coefficient = 1.0f / (1.0f - powf((PROPELLER_RADIUS / (4.0f * dist_to_ground)), 2.0f));
    }
    return ge_coefficient;
}

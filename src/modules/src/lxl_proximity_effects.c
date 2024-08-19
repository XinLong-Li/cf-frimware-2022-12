#include "lxl_proximity_effects.h"
#include <math.h>

#define CEILING_HEIGHT 0.2f // height of the ceiling plane
#define GROUND_HEIGHT 0.0f // default ground height is zero, change according to the height of garage floor
#define NO_CEILING_EFFECT TRUE // TRUE: no ceiling, FALSE: with ceiling
#define NO_GROUND_EFFECT TRUE // TRUE: no ground effect, FALSE: with ground effect

/**
 * @brief Calculate the distance from the propeller plane to the ceiling 
 *         plane，internal used only
 * @param z_position The z-axis position of the quadrotor
 * @return The distance from the propeller plane to the ceiling plane
 */
static float distance_to_ceiling(float z_position)
{
    float dist_to_ceiling = CEILING_HEIGHT - z_position;
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
    float dist_to_ceiling = distance_to_ceiling(z_position); // The unit is meter(m).

    if (NO_CEILING_EFFECT)
    {
        return 1.0f;
    }

    // The nominal minimum distance from propeller plane to ceiling plane,
    // if h_ceiling = 0.1R, Then ce_coefficient = 4.37
    if (dist_to_ceiling <= 0.25f * PROPELLER_RADIUS)
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




/**
 * @brief Calculate the distance from the propeller plane to the ground 
 *         plane，internal used only
 * @param z_position The z-axis position of the quadrotor
 * @return The distance from the propeller plane to the ground plane
 */
static float distance_to_ground(float z_position)
{
    float dist_to_ground = z_position - GROUND_HEIGHT;
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
    float dist_to_ground = distance_to_ground(z_position); // The unit is meter(m).

    if (NO_GROUND_EFFECT)
    {
        return 1.0f;
    }

    // The nominal minimum distance from propeller plane to ground plane,
    // if h_ground = 0.25R, Then ge_coefficient = infinity
    if (dist_to_ground <= 0.5f * PROPELLER_RADIUS)
    {
        ge_coefficient = 2.0;
    }
    else
    {
        ge_coefficient = 1.0f / (1.0f - powf((PROPELLER_RADIUS / (4.0f * dist_to_ground)), 2.0f));
    }
    return ge_coefficient;
}


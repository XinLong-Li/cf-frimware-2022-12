/**
 * @file lxl_proximity_effects.h
 * @brief Proximity effects: ceiling effects and ground effects
 * 
 * @author: Xinlong Li
 * @date 2024.5.7
 *
 *  @note: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *  @todo: Wall effects
 */

#ifndef LXL_PROXIMITY_EFFECTS_H
#define LXL_PROXIMITY_EFFECTS_H

#include <math.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define GRAVITY_ACC 9.81f      // gravity acceleration
#define PROPELLER_RADIUS 0.023f // radius of the crazyflie2.1 propeller
#define QUADROTOR_MASS 0.5f    // mass of the quadrotor

#define NO_CEILING TRUE // TRUE: no ceiling, FALSE: with ceiling
#if defined(NO_CEILING) 
    #define CEILING_HIGHT ((float)INFINITY) // height of the ceiling plane
#else
    #define CEILING_HIGHT 0.2f // height of the ceiling plane
#endif



/**
 * @brief Calculate the ceiling effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ceiling effect coefficient
 */
float ceiling_effect_coefficient(float z_position);

/**
 * @brief Calculate the ground effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ground effect coefficient
 */
float ground_effect_coefficient(float z_position);

#endif // LXL_PROXIMITY_EFFECTS_H

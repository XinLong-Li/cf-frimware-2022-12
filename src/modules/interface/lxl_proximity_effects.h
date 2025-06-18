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

#include "stabilizer_types.h"  // 包含 state_t 的定义

#define ENABLE_INDIVIDUAL_MOTOR_CORRECTION 1  // Set to 1 to enable individual correction for each motor's speed ;  0 disable.


/**
 * @brief Calculate the ceiling effect coefficient based on the position of the quadrotor
 * @param x The x-axis position of the quadrotor (meters)
 * @param y The y-axis position of the quadrotor (meters)
 * @param z The z-axis position of the quadrotor (meters)
 * @return The ceiling effect coefficient
 */
float ceiling_effect_coefficient(float x, float y, float z);


/**
 * @brief Calculate the ground effect coefficient based on the z-axis position of the quadrotor
 * @param x The x-axis position of the quadrotor (meters)
 * @param y The y-axis position of the quadrotor (meters)
 * @param z The z-axis position of the quadrotor (meters)
 * @return The ground effect coefficient
 */
float ground_effect_coefficient(float x, float y, float z);

/**
 * @brief Calculate each propeller's CE coefficient and GE coefficient of a quadrotor
 * @param state The state of the quadrotor
 * @param ce The ceiling effect coefficients for each propeller
 * @param ge The ground effect coefficients for each propeller
 */
void calculate_motor_correction_coefficients(const state_t* state, float sqrt_delta[4]);

#endif // LXL_PROXIMITY_EFFECTS_H

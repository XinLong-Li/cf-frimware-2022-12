/**
 * @file lxl_ground_effect.h
 * @brief Ground effect module header
 * 
 * @date 2024-05-06
 * @author Xinlong Li
 * 
 * @note The prefix of the file name 'lxl_' is the abbreviation of the author's name.
 */

#ifndef LXL_GROUND_EFFECT_H
#define LXL_GROUND_EFFECT_H

/**
 * @brief Calculate the ground effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ground effect coefficient
 */
float ground_effect_coefficient(float z_position);

#endif //__LXL_GROUND_EFFECT_H__

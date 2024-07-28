/*
 * Author: Xinlong Li
 * Date: 2024.5.6
 * Description: the prefix of the file name 'lxl_' is the abbreviation of the author's name
 *
 * lxl_ceiling_effect.h - Ceiling effect module header
 */

#ifndef LXL_CEILING_EFFECT_H
#define LXL_CEILING_EFFECT_H

/**
 * @brief Calculate the ceiling effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ceiling effect coefficient
 */
float ceiling_effect_coefficient(float z_position);

#endif // LXL_CEILING_EFFECT_H

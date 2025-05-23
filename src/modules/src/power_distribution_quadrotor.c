/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2022 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * power_distribution_quadrotor.c - Crazyflie stock power distribution code
 */

#include "power_distribution.h"

#include <string.h>
#include "log.h"
#include "param.h"
#include "num.h"
#include "autoconf.h"
#include "config.h"
#include "lxl_proximity_effects.h"

#ifndef CONFIG_MOTORS_DEFAULT_IDLE_THRUST
#  define DEFAULT_IDLE_THRUST 0
#else
#  define DEFAULT_IDLE_THRUST CONFIG_MOTORS_DEFAULT_IDLE_THRUST
#endif

static uint32_t idleThrust = DEFAULT_IDLE_THRUST;

void powerDistributionInit(void)
{
}

bool powerDistributionTest(void)
{
  bool pass = true;
  return pass;
}

#define limitThrust(VAL) limitUint16(VAL) // 0~65535

// Motor Mixing Algorithm for 'X' Quadrotor
void powerDistribution(motors_thrust_t* motorPower, const control_t *control, const state_t *state)
{
  int16_t r = control->roll / 2.0f; // devide by 2 because this is a 'X' quadrotor
  int16_t p = control->pitch / 2.0f;
  
  if(ENABLE_INDIVIDUAL_MOTOR_CORRECTION)
  { // Correct each motor's speed individually
    float motor_thrust = control->thrust; 
    float sqrt_delta[4] = {1.0f, 1.0f, 1.0f, 1.0f}; //motor speed correction coefficients for each motor
    calculate_motor_correction_coefficients(state, sqrt_delta);
    motorPower->m1 = limitThrust((motor_thrust - r + p + control->yaw) / sqrt_delta[0]);
    motorPower->m2 = limitThrust((motor_thrust - r - p - control->yaw) / sqrt_delta[1]);
    motorPower->m3 = limitThrust((motor_thrust + r - p + control->yaw) / sqrt_delta[2]);
    motorPower->m4 = limitThrust((motor_thrust + r + p - control->yaw) / sqrt_delta[3]);
  }
  else
  { // Correct the total thrust 
    float ce_coefficient = ceiling_effect_coefficient(state->position.x, state->position.y, state->position.z);
    float ge_coefficient = ground_effect_coefficient(state->position.x, state->position.y, state->position.z);
    float motor_thrust = control->thrust / ce_coefficient / ge_coefficient; 
    motorPower->m1 = limitThrust(motor_thrust - r + p + control->yaw);
    motorPower->m2 = limitThrust(motor_thrust - r - p - control->yaw);
    motorPower->m3 = limitThrust(motor_thrust + r - p + control->yaw);
    motorPower->m4 = limitThrust(motor_thrust + r + p - control->yaw);
  }

  if (motorPower->m1 < idleThrust) {
    motorPower->m1 = idleThrust;
  }
  if (motorPower->m2 < idleThrust) {
    motorPower->m2 = idleThrust;
  }
  if (motorPower->m3 < idleThrust) {
    motorPower->m3 = idleThrust;
  }
  if (motorPower->m4 < idleThrust) {
    motorPower->m4 = idleThrust;
  }
}

/**
 * Power distribution parameters
 */
PARAM_GROUP_START(powerDist)
/**
 * @brief Motor thrust to set at idle (default: 0)
 *
 * This is often needed for brushless motors as
 * it takes time to start up the motor. Then a
 * common value is between 3000 - 6000.
 */
PARAM_ADD_CORE(PARAM_UINT32 | PARAM_PERSISTENT, idleThrust, &idleThrust)
PARAM_GROUP_STOP(powerDist)

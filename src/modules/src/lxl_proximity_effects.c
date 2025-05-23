#include "lxl_proximity_effects.h"
#include <math.h>

#define L 0.032f  // distance from the x-axis and y-axis of the body frame to the propeller, unit: meter

#define NO_CEILING_EFFECT 0 // 1: no ceiling, 0: with ceiling
#define NO_GROUND_EFFECT 0  // 1: no ground effect, 0: with ground effect

#define MARKER_HEIGHT 0.027f            // The offset height of the marker center from the bottom of the quadrotor
#define PROPELLER_PLANE_HEIGHT 0.03f    // The offset height of the propeller plane  from the bottom of the quadrotor

// The unit is meter(m)
static const float PROPELLER_RADIUS = 0.023f;                       // The propeller radius of the crazyflie2.1
static const float HANGAR_HEIGHT = 4.0f * PROPELLER_RADIUS;         // The height of the hangar
static const float GROUND_HEIGHT = 0.745f;                          // The height of the hangar ground
static const float CEILING_HEIGHT = 0.835f;  // The height of the hangar ceiling

// The hangar region, the unit is meter(m)
static const float X_ADVC = 0.032f; // Deceleration advance distance, corresponds to response lag
static const float HANGAR_X_MIN =  0.155f;
static const float HANGAR_X_MAX =  0.377f;
static const float HANGAR_Y_MIN = -0.122f;
static const float HANGAR_Y_MAX =  0.293f;


/**
 * @brief Calculate the ceiling effect coefficient based on the position of the quadrotor
 * @param x The x-axis position of the quadrotor (meters)
 * @param y The y-axis position of the quadrotor (meters)
 * @param z The z-axis position of the quadrotor (meters)
 * @return The ceiling effect coefficient
 * 
 * Reference table for h_ceiling (distance from propeller plane to ceiling) and ce_coefficient:
 * | dist_to_ceiling | ce_coefficient |
 * |-----------------|----------------|
 * | 0               | infinity       |
 * | 0.1 * R         | 2.337          |
 * | 0.25 * R        | 1.37           |
 * | 0.5 * R         | 1.11           |
 */

float ceiling_effect_coefficient(float x, float y, float z)
{
    float ce_coefficient;
    float dist_to_ceiling = CEILING_HEIGHT - z;

    if (NO_CEILING_EFFECT) // No considering the ceiling effect
    {
        return 1.0f;
    }

    // if in the hangar region, there exists ceiling effect
    else if (x > (HANGAR_X_MIN - X_ADVC*3) && x < HANGAR_X_MAX && y > HANGAR_Y_MIN && y < HANGAR_Y_MAX)
    {
        ce_coefficient = 0.5f + 0.5f * sqrtf(1.0f + (PROPELLER_RADIUS * PROPELLER_RADIUS) / (8.0f * dist_to_ceiling * dist_to_ceiling));
        // ce_coefficient = (dist_to_ceiling <= 0.25f * PROPELLER_RADIUS) ? 1.37f : ce_coefficient;

        return ce_coefficient;
    }
    else // outside the hangar region, no ceiling effect
    {
        return 1.0f;
    }
}


/**
 * @brief Calculate the ground effect coefficient based on the z-axis position of the quadrotor
 * @param z_position The z-axis position of the quadrotor
 * @return The ground effect coefficient
 * 
 * | dist_to_ground   | ge_coefficient |
 * |------------------|----------------|
 * | 0.25 * R         | infinity       |
 * | 0.5  * R         | 1.33           |
 * | R                | 1.07
 */
float ground_effect_coefficient(float x, float y, float z)
{
    float ge_coefficient;
    float dist_to_ground = z - GROUND_HEIGHT;

    if (NO_GROUND_EFFECT)
    {
        return 1.0f;
    }

    else if (x > HANGAR_X_MIN && x < HANGAR_X_MAX && y > HANGAR_Y_MIN && y < HANGAR_Y_MAX)
    {
         ge_coefficient = 1.0f / (1.0f - powf((PROPELLER_RADIUS / (4.0f * dist_to_ground)), 2.0f));
        // ge_coefficient = (dist_to_ground <= 0.5f * PROPELLER_RADIUS) ? 1.33f : ge_coefficient;

        return ge_coefficient;
    }
    else // outside the hangar region, no ground effect
    {
        return 1.0f;
    }
}


void calculate_motor_correction_coefficients(const state_t* state, float sqrt_delta[4]) {

    float ce[4];
    float ge[4];
    float x = state->attitudeQuaternion.x; 
    float y = state->attitudeQuaternion.y;
    float z = state->attitudeQuaternion.z;
    float w = state->attitudeQuaternion.w; //real part

    // Not sure the order of the quaternion in attitudeQuaternion, which element is the real part
    // float w = state->attitudeQuaternion.x; // real part
    // float x = state->attitudeQuaternion.y;
    // float y = state->attitudeQuaternion.z;
    // float z = state->attitudeQuaternion.w;

    // compute the rotation matrix from quaternion
    float R00 = 1.0f - 2.0f * (y * y + z * z);
    float R01 = 2.0f * (x * y - z * w);
    float R02 = 2.0f * (x * z + y * w);
    
    float R10 = 2.0f * (x * y + z * w);
    float R11 = 1.0f - 2.0f * (x * x + z * z);
    float R12 = 2.0f * (y * z - x * w);
    
    float R20 = 2.0f * (x * z - y * w);
    float R21 = 2.0f * (y * z + x * w);
    float R22 = 1.0f - 2.0f * (x * x + y * y);

    float pos_x = state->position.x;
    float pos_y = state->position.y;
    float pos_z = state->position.z;

    // propeller's position in body frame
    float offset_x[4] = { L,  L, -L, -L };
    float offset_y[4] = { L, -L, -L,  L };
    float offset_z = 0.0f;

    for (int i = 0; i < 4; i++) {
        // compute the world coordinates of the propeller
        float world_x = R00 * offset_x[i] + R01 * offset_y[i] + R02 * offset_z + pos_x;
        float world_y = R10 * offset_x[i] + R11 * offset_y[i] + R12 * offset_z + pos_y;
        float world_z = R20 * offset_x[i] + R21 * offset_y[i] + R22 * offset_z + pos_z;

        ce[i] = ceiling_effect_coefficient(world_x, world_y, world_z);
        ge[i] = ground_effect_coefficient(world_x, world_y, world_z);
        sqrt_delta[i] = sqrtf(ce[i] * ge[i]);
    }
}
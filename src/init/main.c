/**
 *    ||          ____  _ __                           
 * +------+      / __ )(_) /_______________ _____  ___ 
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
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
 * main.c - Containing the main function.
 */

/* Personal configs */
#include "FreeRTOSConfig.h"

/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"

/* Project includes */
#include "config.h"
#include "platform.h"
#include "system.h"
#include "usec_time.h"

#include "led.h"

/* ST includes */
#include "stm32fxxx.h"

#include "bootloader.h"

#define DEBUG_MODULE "My_Motor"
#include "debug.h"

#include "motors.h"

void motor2_task()
{
  DEBUG_PRINT("Waiting for activation ...\n");
  int count_loop = 0;
  uint16_t thrust_cmd = 6000;

  while(1) {
    vTaskDelay(M2T(1000));
    DEBUG_PRINT("My Motor Test!\n");
    DEBUG_PRINT("%d\n",count_loop++);
    motorsSetRatio(MOTOR_M2, thrust_cmd);
  }
}

int main() 
{
  check_enter_bootloader();

  //Initialize the platform.
  int err = platformInit();
  if (err != 0) {
    // The firmware is running on the wrong hardware. Halt
    while(1);
  }

  //Launch the system task that will initialize and start everything
  systemLaunch();


  //test my own task 测试我的任务
  // xTaskCreate(motor2_task, "motor2_task",256, NULL, 1, NULL);




  //Start the FreeRTOS scheduler
  vTaskStartScheduler();

  //TODO: Move to platform launch failed
  ledInit();
  ledSet(0, 1);
  ledSet(1, 1);

  //Should never reach this point!
  while(1);

  return 0;
}


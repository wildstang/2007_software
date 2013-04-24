/*******************************************************************************
* FILE NAME: ws_drive_input.c
*
* DESCRIPTION:
*
* USAGE:
*
*******************************************************************************/

#include <stdio.h>
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "pwm.h"
#include "ws_includes.h"
#include "ws_io.h"
#include "ws_pid.h"
#include "ws_feedback.h"

CrabTgts g_crab_tgts;
extern PidValsType g_crab_front_pid_vals;
extern PidValsType g_crab_back_pid_vals;
UINT8 g_crab_fail_led_state = 0;


/*******************************************************************************
* FUNCTION NAME: feedback_interrupt
* PURPOSE:       runs feedback when the timer fires
*******************************************************************************/
void feedback_interrupt()
{
  /*
  printf("fb crab %d ft %3d bt %3d  ", g_crab_tgts.state,
         g_crab_tgts.front_pot_tgt, g_crab_tgts.back_pot_tgt);
  */

  if (g_crab_tgts.state == FEEDBACK_ENABLED)
  {
#if 1
    show_pid = 0;
    motor_vals.front_crab = crab_feedback(g_crab_tgts.front_pot_tgt,
                                          &calibration_vals.front_crab,
                                          Analog_in_front_crab,
                                          &g_crab_front_pid_vals);
    if((Oi_calibrate > OI_CALIBRATE_ENCODERS) && (Oi_calibrate <
          OI_CALIBRATE_JOYSTICKS))
    {
      if(g_crab_fail_led_state == 1)
      {
        Pwm1_red = 1;
        Pwm2_red = 1;
      }
      else
      {
        Pwm1_red = 0;
        Pwm2_red = 0;
      }
    }
#endif
#if 1
    show_pid = 0;
    motor_vals.back_crab = crab_feedback(g_crab_tgts.back_pot_tgt,
                                         &calibration_vals.back_crab,
                                         Analog_in_back_crab,
                                         &g_crab_back_pid_vals);
    if((Oi_calibrate > OI_CALIBRATE_ENCODERS) && (Oi_calibrate <
          OI_CALIBRATE_JOYSTICKS))
    {
      if(g_crab_fail_led_state == 1)
      {
        Relay1_red = 1;
        Relay2_red = 1;
      }
      else
      {
        Relay1_red = 0;
        Relay2_red = 0;
      }
    }
    show_pid = 0;
#endif

    /*
       printf("spd f %3d b %3d\r", motor_vals.front_crab, motor_vals.back_crab);
     */
  }

  assign_outputs_fast();
  PWM(pwm13,pwm14,pwm15,pwm16);
}


/*******************************************************************************
* FUNCTION NAME: crab_feedback
* PURPOSE:
*******************************************************************************/
INT8 crab_feedback(UINT8 desired_crab_pos_pot, CrabType *calibration,
                   int analog_in, PidValsType *pid_vals)
{
  INT8 crab_speed = 0;
  UINT8 crab_pos;

  /* get current crab position */
  crab_pos = GET_ANALOG_VALUE_SHIFT(analog_in);

#if 0
  printf("des %3d curr %3d ", desired_crab_pos_pot, crab_pos);
#endif

  /* dead pot check (skip for now) */

  crab_speed = ws_pid(pid_vals, crab_pos, desired_crab_pos_pot);

  /* don't allow motors to drive crabs past limit */
  if (((crab_pos > calibration->left) && (crab_speed > 0)) ||
      ((crab_pos < calibration->right) && (crab_speed < 0)))
  {
    crab_speed = 0;
  }

  if((crab_pos >= 245) || (crab_pos <= 10))
  {
    crab_speed = 0;
    g_crab_fail_led_state = 1;
  }
  else
  {
    g_crab_fail_led_state = 0;
  }

#if 0
  printf("%d %d %d\r", crab_pos, crab_speed, Relay1_red);
#endif

  return crab_speed;
}


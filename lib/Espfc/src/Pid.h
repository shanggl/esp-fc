#ifndef _ESPFC_PID_H_
#define _ESPFC_PID_H_

#include "Math.h"

namespace Espfc {

struct PidState
{
  float pTerm;
  float iTerm;
  float dTerm;
  float prevInput;
};

class Pid
{
  public:
    Pid(float kp = 1, float ki = 0, float kd = 0, float il = 0.3, float da = 1, float dg = 0):
      Kp(kp), Ki(ki), Kd(kd), iLimit(il), dAlpha(da), dGamma(dg) {}
    float update(float setpoint, float input, float dt, PidState& state)
    {
      float error = setpoint - input;
      state.pTerm = Kp * error;
      state.iTerm += Ki * error * dt;
      state.iTerm = Math::bound(state.iTerm, -iLimit, iLimit);

      float dTerm = 0;
      if(Kd > 0 && dt > 0)
      {
        dTerm = Kd * (state.prevInput - input) / dt;
      }
      state.dTerm = (1.f - dAlpha) * state.dTerm + dAlpha * dTerm;

      float output = Math::bound(state.pTerm + state.iTerm + state.dTerm, -1.f, 1.f);

      state.prevInput = input;

      return output;
    }

    float Kp;
    float Ki;
    float Kd;
    float iLimit;
    float dAlpha;
    float dGamma;
};

}

#endif

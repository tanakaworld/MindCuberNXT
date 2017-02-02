//-----------------------------------------------------------------------------
// Background task and routines to use color sensor as flashing light
//-----------------------------------------------------------------------------

#define S_CLR     IN_3

#define CLR_R     0
#define CLR_B     1
#define CLR_O     2
#define CLR_G     3
#define CLR_W     4
#define CLR_Y     5

#define L_DEL    1000

mutex light_mtx;
bool  light_flash = false;
int   light_clr   = CLR_R;

void color_sensor_on() {
#ifdef HITECHNIC_V2
  SetSensorLowspeed(S_CLR, true);
#else
  SetSensorColorFull(S_CLR);
  ResetSensor(S_CLR);
#endif
}

void color_sensor_off() {
#ifdef HITECHNIC_V2
  SetSensorLowspeed(S_CLR, false);
#else
  SetSensorType(S_CLR, SENSOR_TYPE_NONE);
#endif
}

task light_task() {
  long ms = CurrentTick();
  Acquire(light_mtx);
  while (light_flash) {
#ifdef HITECHNIC_V2
#else
    switch (light_clr) {
    case CLR_R: SetSensorColorRed(S_CLR);   break;
    case CLR_G: SetSensorColorGreen(S_CLR); break;
    case CLR_B: SetSensorColorBlue(S_CLR);  break;
    }
#endif
    color_sensor_off();
    Release(light_mtx);
    ms += L_DEL;
    long del = ms-CurrentTick();
    if (del < 1)
      del = 1;
    else if (del > L_DEL)
      del = L_DEL;
    Wait(del);
    Acquire(light_mtx);
  }
  Release(light_mtx);
}

void flash_on(int clr) {
  Acquire(light_mtx);
  light_clr = clr;
  if (!light_flash) {
    light_flash = true;
    start light_task;
  }
  Release(light_mtx);
}

void flash_red() {
  flash_on(CLR_R);
}

void flash_green() {
  flash_on(CLR_G);
}

void flash_blue() {
  flash_on(CLR_B);
}

void flash_off() {
  Acquire(light_mtx);
  light_flash = false;
  Release(light_mtx);
  color_sensor_off();
}

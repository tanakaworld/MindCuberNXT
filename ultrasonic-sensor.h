//-----------------------------------------------------------------------------
// Routines to detect the presence of a cube using the ultrasonic sensor
//-----------------------------------------------------------------------------

#define S_ULTRA   IN_2

#define CD_DIST   18
#define CD_DELAY  200
#define CD_COUNT  5

bool cd_on    = false;
long cd_time  = 0;
int  cd_count = 0;

bool check_cube_detect() {
  if (!cd_on) {
    cd_on    = true;
    cd_time  = CurrentTick();
    cd_count = 0;
    SetSensorUltrasonic(S_ULTRA);
    ResetSensor(S_ULTRA);
  }
  long diff = CurrentTick()-cd_time;
  if (diff > 0) {
    cd_time += CD_DELAY;
    return true;
  }
  return false;
}

void cube_detect_off() {
  cd_on = false;
  SetSensorType(S_ULTRA, SENSOR_TYPE_NONE);
}

bool cube_present() {
  if (check_cube_detect()) {
    if (SensorUS(S_ULTRA) < CD_DIST)
      cd_count ++;
    else
      cd_count = 0;
  }
  return cd_count >= CD_COUNT;
}

bool cube_absent() {
  if (check_cube_detect()) {
    if (SensorUS(S_ULTRA) >= CD_DIST)
      cd_count ++;
    else
      cd_count = 0;
  }
  return cd_count >= CD_COUNT;
}

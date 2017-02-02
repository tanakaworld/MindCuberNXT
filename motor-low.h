//-----------------------------------------------------------------------------
// Background task and low level routines to control motors
//-----------------------------------------------------------------------------

#define NMOTORS 3

#define M_DELAY 10
#define M_SCALE 12
#define AMAX    24
#define VMAX    100

#define P_LOW   35
#define P_HIGH  87

mutex      motor_mtx;
const byte mmot[] = {OUT_A, OUT_B, OUT_C};
bool       mon[]  = {false, false, false};
bool       mgo[]  = {false, false, false};
bool       mup[]  = {false, false, false};
long       mtx[]  = {0, 0, 0};
long       mx[]   = {0, 0, 0};
long       mv[]   = {0, 0, 0};
long       ma[]   = {0, 0, 0};
long       mp[]   = {0, 0, 0};
long       me[]   = {0, 0, 0};

void motors_on() {
  Acquire(motor_mtx);
  for (int m = 0; m < NMOTORS; m++)
    mon[m] = true;
  Release(motor_mtx);
}

void motors_off() {
  bool on;
  do {
    on = false;
    Acquire(motor_mtx);
    for (int m = 0; m < NMOTORS; m++) {
      if (mon[m]) {
        if (mgo[m]) {
          on = true;
        } else {
          byte mot = mmot[m];
          OffEx(mot, RESET_NONE);
          mon[m] = false;
          mv[m]  = 0;
          ma[m]  = 0;
          mp[m]  = 0;
        }
      }
    }
    Release(motor_mtx);
    Wait(1);
  } while (on);
}

void endstop(int m) {
  long p0, p1;
  byte mot = mmot[m];
  Acquire(motor_mtx);
  mon[m] = false;
  Release(motor_mtx);
  OnFwdEx(mot, P_LOW, RESET_NONE);
  p1 = MotorTachoCount(mot);
  do {
    Wait(500);
    p0 = p1;
    p1 = MotorTachoCount(mot);
  } while (p0 < p1);
  ResetTachoCount(mot);
  Wait(200);
}

task motor_task() {
  long ms = CurrentTick();
  while (true) {
    Acquire(motor_mtx);
    for (int m = 0; m < NMOTORS; m++) {
      if (mon[m]) {
        bool rev = false;
        byte mot = mmot[m];
        long x = mx[m];
        long v = mv[m];
        long a = ma[m];
        long p = mp[m];
        long e = 0;
        long ax = M_SCALE*MotorTachoCount(mot);
        long ex = ax-x;
        if (-M_SCALE < ex && ex < M_SCALE)
          ax = x;
        else if (mgo[m])
          e  = me[m]-ex;
        long d = mtx[m]-ax;
        if (mup[m] ? (d < M_SCALE) : (d > -M_SCALE)) {
          mgo[m] = false;
          e = 0;
        }
        if (d < 0) {
          d = -d;
          v = -v;
          a = -a;
          p = -p;
          e = -e;
          rev = true;
        }
        if (d >= M_SCALE) {
          if (v >= 0) {
            long dd = (v+AMAX/2)+(v*v)/(2*AMAX);
            if (d >= dd) {
              p = P_HIGH;
              a = (AMAX*(VMAX-v))/VMAX;
              e = 0;
            } else {
              a = -(v*v)/(2*d);
              if (a < -v)
                a = -v;
              if (a < -AMAX)
                a = -AMAX;
              p = (P_HIGH*a*(VMAX-v))/(AMAX*VMAX);
            }
          } else {
            a = -v;
            if (a > AMAX)
              a = AMAX;
            p = (P_HIGH*a*(VMAX+v))/(AMAX*VMAX);
          }
        } else {
          a = -v;
          if (a < -AMAX)
            a = -AMAX;
          else if (a > AMAX)
            a = AMAX;
          p = (P_HIGH*a)/AMAX;
        }
        d = v+a/2;
        v += a;
        p += e;
        if (p > P_HIGH) {
          p = P_HIGH;
          e = 0;
        } else if (p < -P_HIGH) {
          p = -P_HIGH;
          e = 0;
        }
        if (rev) {
          d = -d;
          v = -v;
          a = -a;
          p = -p;
          e = -e;
        }
        if (p != mp[m]) {
          if (p > 0)
            OnFwdEx(mot, p, RESET_NONE);
          else if (p < 0)
            OnRevEx(mot, -p, RESET_NONE);
          else
            OffEx(mot, RESET_NONE);
          mp[m] = p;
        }
        mx[m] = ax+d;
        mv[m] = v;
        ma[m] = a;
        me[m] = e;
      }
    }
    Release(motor_mtx);
    ms += M_DELAY;
    long del = ms-CurrentTick();
    if (del < 1)
      del = 1;
    else if (del > M_DELAY)
      del = M_DELAY;
    Wait(del);
  }
}

void move_wait(int m) {
  bool go;
  do {
    Wait(1);
    Acquire(motor_mtx);
    go = mgo[m];
    Release(motor_mtx);
  } while (go);
}

void move_abs(int m, long pos) {
  Acquire(motor_mtx);
  mtx[m] = -M_SCALE*pos;
  mup[m] = (mtx[m] > mx[m]);
  mon[m] = true;
  mgo[m] = true;
  Release(motor_mtx);
}

void move_rel(int m, long pos, long off = 0) {
  Acquire(motor_mtx);
  mtx[m] += -M_SCALE*(pos+off);
  mup[m] = (mtx[m] > mx[m]);
  mon[m] = true;
  mgo[m] = true;
  Release(motor_mtx);
  if (off != 0) {
    move_wait(m);
    Acquire(motor_mtx);
    mtx[m] -= -M_SCALE*off;
    mup[m] = (mtx[m] > mx[m]);
    mgo[m] = true;
    Release(motor_mtx);
  }
}

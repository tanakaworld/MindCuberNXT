//-----------------------------------------------------------------------------
// Routines for high level motor control for turn, scan and tilt movements
//-----------------------------------------------------------------------------

#define M_TURN  0
#define M_SCAN  1
#define M_TILT  2

#ifdef NXT_8527
#define T_90    210
#define T_CUT   7
#define T_OVR   48
#ifdef HITECHNIC_V2
#define T_SOFF  0
#define T_SCNT  15
#define T_SCNR  100
#define T_SEDG  83
#else
#define T_SOFF  12
#define T_SCNT  15
#define T_SCNR  105
#define T_SEDG  88
#endif
#define T_TILT  85
#define T_TAWY  25
#define T_TREL  62
#define T_THLD  160
#define T_ADJ   2
#else
#define T_90    270
#define T_CUT   10
#define T_OVR   57
#ifdef HITECHNIC_V2
#define T_SOFF  0
#define T_SCNT  10
#define T_SCNR  103
#define T_SEDG  87
#else
#define T_SOFF  10
#define T_SCNT  10
#define T_SCNR  103
#define T_SEDG  87
#endif
#define T_TILT  85
#define T_TAWY  25
#define T_TREL  62
#define T_THLD  160
#define T_ADJ   3
#endif

int apply_sign(int n, int d) {
  return (n < 0) ? -d : ((n > 0) ? d : 0);
}

void turn(int r) {
  move_rel(M_TURN, r*T_90);
  move_wait(M_TURN);
}

void turn45(void) {
  move_rel(M_TURN, -T_90/2);
}

void turn_face(int r, int rn) {
  move_rel(M_TURN, r*T_90, apply_sign(r, T_OVR)+apply_sign(rn, T_CUT));
  move_wait(M_TURN);
}

void scan_to_center() {
  move_abs(M_SCAN, T_SCNT);
  move_wait(M_SCAN);
}

void scan_to_corner() {
  move_abs(M_SCAN, T_SCNR);
}

void scan_to_edge() {
  move_abs(M_SCAN, T_SEDG);
}

void scan_away() {
  move_abs(M_SCAN, 200);
}

void scan_wait() {
  move_wait(M_SCAN);
}

void scan_turn_wait() {
  move_wait(M_SCAN);
  move_wait(M_TURN);
}

bool holding = false;

void tilt_away() {
  holding = false;
  move_abs(M_TILT, T_TAWY);
  move_wait(M_TILT);
}

void tilt_release() {
  holding = false;
  move_abs(M_TILT, T_TREL);
  move_wait(M_TILT);
}

void tilt_hold() {
  holding = true;
  move_abs(M_TILT, T_THLD);
  move_wait(M_TILT);
}

void tilt(int n = 1) {
  if (holding) {
    Wait(100);
  } else {
    tilt_hold();
    Wait(350);
  }
  for (int i = 0; i < n; i++) {
    if (i > 0) {
      move_wait(M_TILT);
      Wait(500);
    }
    int t = T_TILT;
    move_rel(M_TILT, t);
    move_wait(M_TILT);
    Wait(150);
    move_rel(M_TILT, -t, -25);
    move_wait(M_TILT);
  }
}

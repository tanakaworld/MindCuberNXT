
//-----------------------------------------------------------------------------
// Routines to detect button presses with auto-repeat when held
//-----------------------------------------------------------------------------

#define BTN_AUTO_REPEAT_FIRST   1000
#define BTN_AUTO_REPEAT         30

long btn_time   = 0;
bool btn_left   = false;
bool btn_right  = false;
bool btn_center = false;

bool btn_auto_repeat(byte btn, bool &btn_pressed) {
  if (ButtonPressed(btn, false)) {
    if (btn_pressed) {
      long diff = CurrentTick()-btn_time;
      if (diff >= 0) {
        btn_time += BTN_AUTO_REPEAT;
        return true;
      }
    } else {
      btn_pressed = true;
      btn_time = CurrentTick()+BTN_AUTO_REPEAT_FIRST;
      return true;
    }
  } else
    btn_pressed = false;
  return false;
}

bool left_pressed() {
  return btn_auto_repeat(BTNLEFT, btn_left);
}

bool right_pressed() {
  return btn_auto_repeat(BTNRIGHT, btn_right);
}

bool center_pressed() {
  return btn_auto_repeat(BTNCENTER, btn_center);
}

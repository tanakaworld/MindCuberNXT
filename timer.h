//-----------------------------------------------------------------------------
// Background task and routines to display timer during solve
//-----------------------------------------------------------------------------

mutex  tmr_mtx;
bool   tmr_on   = false;
long   tmr_time = 0;
string tmr_str  = "      ";

string ms_to_str(long ms) {
  long t = ms / 100;
  long s = t / 10;
  t %= 10;
  long m = s / 60;
  s %= 60;
  return NumToStr(m)+":"+NumToStr(s/10)+NumToStr(s%10)+"."+NumToStr(t);
}

task timer_task() {
  bool on = false;
  do {
    long del = 100;
    Acquire(tmr_mtx);
    on = tmr_on;
    if (on) {
      long t = CurrentTick()-tmr_time;
      del -= (t % 100);
      tmr_str = ms_to_str(t);
      TextOut(0, LCD_LINE8, tmr_str);
    }
    Release(tmr_mtx);
    Wait(del);
  } while (on);
}

void timer_start() {
  Acquire(tmr_mtx);
  if (!tmr_on) {
    tmr_on   = true;
    tmr_time = CurrentTick();
    start timer_task;
  }
  Release(tmr_mtx);
}

void timer_stop() {
  Acquire(tmr_mtx);
  tmr_on = false;
  Release(tmr_mtx);
}

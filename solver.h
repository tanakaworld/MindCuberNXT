//-----------------------------------------------------------------------------
// High level routines to scan, solve and manipulate cube
//-----------------------------------------------------------------------------

byte clr_map[] = {0, 1, 2, 3, 4, 5};
int  clr_ord[NFACE*4];

void determine_colors(byte &cube[], const int t) {
  for (int i = 0; i < NFACE; i++)
    clr_ord[i] = 9*i+8;
  sort_colors(clr_ord, t, 1);
  for (int i = 0; i < NFACE; i++) {
    clr_ord[4*i+0] = 9*i+0;
    clr_ord[4*i+1] = 9*i+2;
    clr_ord[4*i+2] = 9*i+4;
    clr_ord[4*i+3] = 9*i+6;
  }
  sort_colors(clr_ord, t, 4);
  for (int i = 0; i < NFACE; i++) {
    clr_ord[4*i+0] = 9*i+1;
    clr_ord[4*i+1] = 9*i+3;
    clr_ord[4*i+2] = 9*i+5;
    clr_ord[4*i+3] = 9*i+7;
  }
  sort_colors(clr_ord, t, 4);
  for (int f = 0; f < NFACE; f++)
    clr_map[sc_clr[f*9+8]] = f;
  clr_chr[clr_map[CLR_R]] = CHR_R;
  clr_chr[clr_map[CLR_B]] = CHR_B;
  clr_chr[clr_map[CLR_O]] = CHR_O;
  clr_chr[clr_map[CLR_G]] = CHR_G;
  clr_chr[clr_map[CLR_W]] = CHR_W;
  clr_chr[clr_map[CLR_Y]] = CHR_Y;
  for (int f = 0; f < NFACE; f++)
    for (int o = 0; o < 8; o++)
      cube[POS(f, o)] = clr_map[sc_clr[f*9+o]];
}

int  uc = U;
int  fc = F;

int pieces_valid = 0;
int valid_i = 0;

bool scan_solve_cube(byte &cube[]) {
  flash_off();
  color_sensor_on();
  scan_face(1, 3, 2);
  tilt();
  scan_face(2, 2, 2);
  tilt();
  scan_face(3, 1, 2);
  tilt();
  scan_face(4, 0, 2);
  turn(-1);
  tilt();
  scan_face(5, 4, 6);
  tilt(2);
  scan_face(6, 5, 2);
  motors_off();
  flash_red();
  bool solved = false;
  ClearScreen();
  TextOut(0*6, LCD_LINE1, "Processing...");
  for (int i = 0; i < 6; i++) {
    determine_colors(cube, i);
    if (valid_pieces(cube)) {
      pieces_valid ++;
      valid_i = i;
      display_cube(cube);
      if (solve(cube)) {
        solved = true;
        break;
      }
    }
  }
  motors_on();
  ClearScreen();
  display_cube(cube);
  if (solved) {
    TextOut(0, LCD_LINE1, "Solving...");
    scan_away();
    flash_off();
    scan_wait();
  } else {
    TextOut(0, LCD_LINE1, "Scan error...");
    if (pieces_valid > 1) {
      TextOut(0, LCD_LINE1, "Cube");
      flash_blue();
    } else {
      flash_red();
    }
  }
  uc = L;
  fc = D;
  return solved;
}

void manipulate(byte &cube[], int f, int r, int rn) {
  int map = MAP(uc, fc);
  if (fmap[map+U] == f) {
    uc = fmap[map+D]; fc = fmap[map+B];
    Wait(50);
    tilt(2);
  } else if (fmap[map+F] == f) {
    uc = fmap[map+B]; fc = fmap[map+U];
    Wait(50);
    tilt();
  } else if (fmap[map+D] == f) {
    tilt_hold();
  } else if (fmap[map+B] == f) {
    uc = fmap[map+F]; fc = fmap[map+U];
    tilt_release();
    turn(2);
    tilt();
  } else if (fmap[map+R] == f) {
    uc = fmap[map+L]; fc = fmap[map+U];
    tilt_release();
    turn(1);
    tilt();
  } else { // fmap[map+L] == f
    uc = fmap[map+R]; fc = fmap[map+U];
    tilt_release();
    turn(-1);
    tilt();
  }
  Wait(150);
  rotate(cube, f, r);
  display_cube(cube);
  turn_face(-r, -rn);
}

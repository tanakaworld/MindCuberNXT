//-----------------------------------------------------------------------------
// Routines to scan a face of the cube and calibrate white balance
//-----------------------------------------------------------------------------

long         white_rgb[] = {910, 960, 1024};
unsigned int rgbn[INPUT_NO_OF_COLORS];

void sample_rgb(int &rgb[], int f, int o, bool cal) {
  const int scan_max = 5;
  Wait(10);
  for (int i = 0; i < 3; i++)
    rgb[i] = 0;
  for (int n = 0; n < scan_max; n++) {
#ifdef HITECHNIC_V2
    unsigned int r, g, b, w;
    do {
      Wait(1);
    } while (!ReadSensorHTRawColor2(S_CLR, r, g, b, w));
    rgbn[INPUT_RED]   = r / 4;
    rgbn[INPUT_GREEN] = g / 64;
    rgbn[INPUT_BLUE]  = (b > 20000) ? (b-20000)/48 : 0;
    rgbn[INPUT_BLANK] = w / 64;
#else
    do {
      Wait(1);
    } while (!ReadSensorColorRaw(S_CLR, rgbn));
#endif
    rgb[0] += rgbn[INPUT_RED];
    rgb[1] += rgbn[INPUT_GREEN];
    rgb[2] += rgbn[INPUT_BLUE];
    log(rgbn[INPUT_RED]);
    log(rgbn[INPUT_GREEN]);
    log(rgbn[INPUT_BLUE]);
    log(rgbn[INPUT_BLANK]);
    logeol();
  }
  for (int i = 0; i < 3; i++) {
    if (cal) {
      rgb[i] /= scan_max;
      white_rgb[i] += rgb[i];
    } else {
      rgb[i] = (white_rgb[i]*rgb[i])/(scan_max*cmax);
    }
  }
}

int scan_rgb[3];

void scan_face(int n, int f, int o, bool cal = false) {
  TextOut(0, LCD_LINE6, "Face");
  NumOut(5*6, LCD_LINE6, n);
  log(f);
  logeol();
  tilt_release();
  move_rel(M_TURN, T_SOFF);
  scan_to_center();
  sample_rgb(scan_rgb, f, 8, cal);
  int orgb = f*9+8;
  for (int i = 0; i < 4; i++) {
    turn45();
    scan_to_corner();
    set_rgb(orgb, scan_rgb);
    scan_turn_wait();
    sample_rgb(scan_rgb, f, o, cal);
    orgb = f*9+o;
    turn45();
    scan_to_edge();
    set_rgb(orgb, scan_rgb);
    scan_turn_wait();
    sample_rgb(scan_rgb, f, o+1, cal);
    orgb += 1;
    o = (o+2)&7;
  }
  move_rel(M_TURN, -T_SOFF);
  if (n != 6)
    scan_away();
  set_rgb(orgb, scan_rgb);
}

string cfg_name = "mccfg.dat";
byte   cfgh;

void calibrate_white() {
  flash_off();
  color_sensor_on();
  for (int i = 0; i < 3; i++)
    white_rgb[i] = 0;
  scan_face(1, 3, 2, true);
  long cmin = white_rgb[0];
  for (int i = 1; i < 3; i++) {
    if (white_rgb[i] < cmin)
      cmin = white_rgb[i];
  }
  for (int i = 0; i < 3; i++) {
    white_rgb[i] = (cmax*cmin)/white_rgb[i];
    NumOut(5*i*6, LCD_LINE5, white_rgb[i]);
  }
  // Save white calibration value to a configuration file
  DeleteFile(cfg_name);
  if (CreateFile(cfg_name, 100, cfgh) == LDR_SUCCESS) {
    Write(cfgh, white_rgb);
    CloseFile(cfgh);
  }
  flash_blue();
}

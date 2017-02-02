//-----------------------------------------------------------------------------
// Routines to display, transform and solve a cube
//-----------------------------------------------------------------------------

// Target moves for solve to finish early to save time
// Average: 42 moves in 5 seconds
// Range:   about 37 to 47 moves in 1 to 10 seconds

#define TARGET  42

#define NPIECE   3    // maximum number of pieces indexed in one phase
#define MV_MAX   100  // maximum number of moves per solve

#define RFIX(RR) ((((RR)+1)&3)-1)  // Normalise to range -1 to 2

#define CHR_R    'r'
#define CHR_B    'b'
#define CHR_O    'o'
#define CHR_G    'g'
#define CHR_W    'w'
#define CHR_Y    'y'

char clr_chr[] = {CHR_R, CHR_B, CHR_O, CHR_G, CHR_W, CHR_Y};

void display_face(int x, int y, byte &cube[], int f, int o) {
  byte str[4];
  str[3] = 0;
  int b = f * 8;
  str[0] = clr_chr[cube[b+  o      ]];
  str[1] = clr_chr[cube[b+ (o+1)   ]];
  str[2] = clr_chr[cube[b+((o+2)&7)]];
  TextOut(x, y, ByteArrayToStr(str));
  str[0] = clr_chr[cube[b+((o+7)&7)]];
  str[1] = clr_chr[f];
  str[2] = clr_chr[cube[b+((o+3)&7)]];
  TextOut(x, y+LCD_LINE_OFF, ByteArrayToStr(str));
  str[0] = clr_chr[cube[b+((o+6)&7)]];
  str[1] = clr_chr[cube[b+((o+5)&7)]];
  str[2] = clr_chr[cube[b+((o+4)&7)]];
  TextOut(x, y+2*LCD_LINE_OFF, ByteArrayToStr(str));
}

void display_cube(byte &cube[]) {
  display_face(0,  LCD_LINE3, cube, U, 2);
  display_face(19, LCD_LINE3, cube, F, 2);
  display_face(38, LCD_LINE3, cube, D, 2);
  display_face(38, LCD_LINE6, cube, L, 4);
  display_face(57, LCD_LINE6, cube, B, 0);
  display_face(76, LCD_LINE6, cube, R, 4);
}

void init_cube(byte &cube[]) {
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++)
      cube[o++] = f;
}

void copy_cube(byte &cube0[], byte &cube1[]) {
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++) {
      cube0[o] = cube1[o];
      o ++;
    }
}

bool solved(byte &cube[]) {
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++) {
      if (cube[o++] != f)
        return false;
    }
  return true;
}

int  mv_n;
byte mv_f[MV_MAX];
int  mv_r[MV_MAX];

const byte opposite[] = {D, B, U, F, L, R};

void add_mv(int f, int r) {
  int  i   = mv_n;
  bool mrg = false;
  while (i > 0) {
    i--;
    int fi = mv_f[i];
    if (f == fi) {
      r += mv_r[i];
      r = RFIX(r);
      if (r != 0) {
        mv_r[i] = r;
      } else {
        mv_n --;
        while (i < mv_n) {
          mv_f[i] = mv_f[i+1];
          mv_r[i] = mv_r[i+1];
          i++;
        }
      }
      mrg = true;
      break;
    }
    if (opposite[f] != fi)
      break;
  }
  if (!mrg) {
    mv_f[mv_n] = f;
    mv_r[mv_n] = RFIX(r);
    mv_n++;
  }
}

void rot_edges(byte &cube[], int r,
               int f0, int o0, int f1, int o1,
               int f2, int o2, int f3, int o3) {
  f0 *= 8;
  f1 *= 8;
  f2 *= 8;
  f3 *= 8;
  o0 += f0;
  o1 += f1;
  o2 += f2;
  o3 += f3;
  byte p;
  switch (r) {
  case 1:
    p = cube[o3];
    cube[o3] = cube[o2];
    cube[o2] = cube[o1];
    cube[o1] = cube[o0];
    cube[o0] = p;
    o0 ++; o1 ++; o2 ++; o3 ++;
    p = cube[o3];
    cube[o3] = cube[o2];
    cube[o2] = cube[o1];
    cube[o1] = cube[o0];
    cube[o0] = p;
    o0 = f0+((o0+1)&7); o1 = f1+((o1+1)&7);
    o2 = f2+((o2+1)&7); o3 = f3+((o3+1)&7);
    p = cube[o3];
    cube[o3] = cube[o2];
    cube[o2] = cube[o1];
    cube[o1] = cube[o0];
    cube[o0] = p;
    break;

  case 2:
    p = cube[o0];
    cube[o0] = cube[o2];
    cube[o2] = p;
    p = cube[o1];
    cube[o1] = cube[o3];
    cube[o3] = p;
    o0 ++; o1 ++; o2 ++; o3 ++;
    p = cube[o0];
    cube[o0] = cube[o2];
    cube[o2] = p;
    p = cube[o1];
    cube[o1] = cube[o3];
    cube[o3] = p;
    o0 = f0+((o0+1)&7); o1 = f1+((o1+1)&7);
    o2 = f2+((o2+1)&7); o3 = f3+((o3+1)&7);
    p = cube[o0];
    cube[o0] = cube[o2];
    cube[o2] = p;
    p = cube[o1];
    cube[o1] = cube[o3];
    cube[o3] = p;
    break;

  case 3:
    p = cube[o0];
    cube[o0] = cube[o1];
    cube[o1] = cube[o2];
    cube[o2] = cube[o3];
    cube[o3] = p;
    o0 ++; o1 ++; o2 ++; o3 ++;
    p = cube[o0];
    cube[o0] = cube[o1];
    cube[o1] = cube[o2];
    cube[o2] = cube[o3];
    cube[o3] = p;
    o0 = f0+((o0+1)&7); o1 = f1+((o1+1)&7);
    o2 = f2+((o2+1)&7); o3 = f3+((o3+1)&7);
    p = cube[o0];
    cube[o0] = cube[o1];
    cube[o1] = cube[o2];
    cube[o2] = cube[o3];
    cube[o3] = p;
    break;

  default:
    error("rot_edges", r);
  }
}

void rotate(byte &cube[], int f, int r) {
  r &= 3;
  switch (f) {
  case U:  rot_edges(cube, r, B, 4, R, 0, F, 0, L, 0); break;
  case F:  rot_edges(cube, r, U, 4, R, 6, D, 0, L, 2); break;
  case D:  rot_edges(cube, r, F, 4, R, 4, B, 0, L, 4); break;
  case B:  rot_edges(cube, r, D, 4, R, 2, U, 0, L, 6); break;
  case R:  rot_edges(cube, r, U, 2, B, 2, D, 2, F, 2); break;
  case L:  rot_edges(cube, r, U, 6, F, 6, D, 6, B, 6); break;
  default: error("rot face", f);
  }
  f *= 8;
  byte p;
  switch (r) {
  case 1:
    p         = cube[f+7];
    cube[f+7] = cube[f+5];
    cube[f+5] = cube[f+3];
    cube[f+3] = cube[f+1];
    cube[f+1] = p;
    p         = cube[f+6];
    cube[f+6] = cube[f+4];
    cube[f+4] = cube[f+2];
    cube[f+2] = cube[f];
    cube[f]   = p;
    break;

  case 2:
    p         = cube[f+1];
    cube[f+1] = cube[f+5];
    cube[f+5] = p;
    p         = cube[f+3];
    cube[f+3] = cube[f+7];
    cube[f+7] = p;
    p         = cube[f];
    cube[f]   = cube[f+4];
    cube[f+4] = p;
    p         = cube[f+2];
    cube[f+2] = cube[f+6];
    cube[f+6] = p;
    break;

  case 3:
    p         = cube[f+1];
    cube[f+1] = cube[f+3];
    cube[f+3] = cube[f+5];
    cube[f+5] = cube[f+7];
    cube[f+7] = p;
    p         = cube[f];
    cube[f]   = cube[f+2];
    cube[f+2] = cube[f+4];
    cube[f+4] = cube[f+6];
    cube[f+6] = p;
    break;

  default:
    error("rot", r);
  }
}

int idx_ic;
int idx_ie;
int idx_idx[NPIECE];
int idx_nc;
int idx_ne;
int idx;

void index_init() {
  idx_nc = 0;
  idx_ne = 0;
  idx = 0;
}

void index_last() {
  idx = ((idx>>2)<<1)|(idx&1);
}

#define FIND_CORNER(F0, O0, F1, O1, F2, O2, I0, I1, I2) \
  c0 = cube[POS(F0, O0)]; \
  if (c0 == f0) { \
    if (cube[POS(F1, O1)] == f1 && \
        cube[POS(F2, O2)] == f2) return I2; \
  } else if (c0 == f2) { \
    if (cube[POS(F1, O1)] == f0 && \
        cube[POS(F2, O2)] == f1) return I1; \
  } else if (c0 == f1) { \
    if (cube[POS(F1, O1)] == f2 && \
        cube[POS(F2, O2)] == f0) return I0; \
  }

int find_corner(byte &cube[], byte f0, byte f1, byte f2) {
  byte c0;
  FIND_CORNER(U, 2, B, 4, R, 2, 0,  1,  2);
  FIND_CORNER(U, 0, L, 0, B, 6, 3,  4,  5);
  FIND_CORNER(U, 6, F, 0, L, 2, 6,  7,  8);
  FIND_CORNER(U, 4, R, 0, F, 2, 9,  10, 11);
  FIND_CORNER(D, 0, L, 4, F, 6, 12, 13, 14);
  FIND_CORNER(D, 6, B, 0, L, 6, 15, 16, 17);
  FIND_CORNER(D, 4, R, 4, B, 2, 18, 19, 20);
  FIND_CORNER(D, 2, F, 4, R, 6, 21, 22, 23);
  return -1;
}

int index_corner(byte &cube[], byte f0, byte f1, byte f2) {
  int ic = find_corner(cube, f0, f1, f2);
  for (int i = 0; i < idx_nc; i++) {
    if (ic > idx_idx[i])
      ic -= 3;
  }
  idx = (idx*idx_ic) + ic;
  idx_idx[idx_nc++] = ic;
  idx_ic -= 3;
}

#define FIND_EDGE(F0, O0, F1, O1, I0, I1) \
  e0 = cube[POS(F0, O0)]; \
  if (e0 == f0) { \
    if (cube[POS(F1, O1)] == f1) return I1; \
  } else if (e0 == f1) { \
    if (cube[POS(F1, O1)] == f0) return I0; \
  }

int find_edge(byte &cube[], byte f0, byte f1) {
  byte e0;
  FIND_EDGE(U, 1, B, 5, 0,  1);
  FIND_EDGE(U, 7, L, 1, 2,  3);
  FIND_EDGE(U, 5, F, 1, 4,  5);
  FIND_EDGE(U, 3, R, 1, 6,  7);
  FIND_EDGE(L, 3, F, 7, 8,  9);
  FIND_EDGE(B, 7, L, 7, 10, 11);
  FIND_EDGE(D, 7, L, 5, 12, 13);
  FIND_EDGE(R, 3, B, 3, 14, 15);
  FIND_EDGE(D, 5, B, 1, 16, 17);
  FIND_EDGE(F, 3, R, 7, 18, 19);
  FIND_EDGE(D, 3, R, 5, 20, 21);
  FIND_EDGE(D, 1, F, 5, 22, 23);
  return -1;
}

void index_edge(byte &cube[], byte f0, byte f1) {
  int ie = find_edge(cube, f0, f1);
  for (int i = 0; i < idx_ne; i++) {
    if (ie > idx_idx[i])
      ie -= 2;
  }
  idx = (idx*idx_ie) + ie;
  idx_idx[idx_ne++] = ie;
  idx_ie -= 2;
}

bool valid_pieces(byte &cube[]) {
  return
    (find_edge(cube, U, F) >= 0) &&
    (find_edge(cube, U, L) >= 0) &&
    (find_edge(cube, U, B) >= 0) &&
    (find_edge(cube, U, R) >= 0) &&
    (find_edge(cube, F, L) >= 0) &&
    (find_edge(cube, L, B) >= 0) &&
    (find_edge(cube, B, R) >= 0) &&
    (find_edge(cube, R, F) >= 0) &&
    (find_edge(cube, D, F) >= 0) &&
    (find_edge(cube, D, L) >= 0) &&
    (find_edge(cube, D, B) >= 0) &&
    (find_edge(cube, D, R) >= 0) &&
    (find_corner(cube, U, F, L) >= 0) &&
    (find_corner(cube, U, L, B) >= 0) &&
    (find_corner(cube, U, B, R) >= 0) &&
    (find_corner(cube, U, R, F) >= 0) &&
    (find_corner(cube, D, F, R) >= 0) &&
    (find_corner(cube, D, R, B) >= 0) &&
    (find_corner(cube, D, B, L) >= 0) &&
    (find_corner(cube, D, L, F) >= 0);
}

void solve_phase(byte &cube[], int mtb, const byte &mtd[], bool dorot = true) {
  int sz = ArrayLen(mtd)/mtb;
  idx = sz-idx;
  if (idx > 0) {
    int i = (idx-1)*mtb;
    byte b = mtd[i++];
    if (b != 0xFF) {
      const int mvm = mtb*2-1;
      int mv = 0;
      int f0 = b / 3;
      int r0 = RFIX(b-(f0*3)+1);
      add_mv(f0, r0);
      if (dorot)
        rotate(cube, f0, r0);
      mv ++;
      while (mv < mvm) {
        b >>= 4;
        if ((mv & 1) != 0)
          b = mtd[i++];
        byte b0 = b & 0xF;
        if (b0 == 0xF)
          break;
        int f1 = b0 / 3;
        r0 = RFIX(b0-(f1*3)+1);
        if (f1 >= f0)
          f1 ++;
        f0 = f1;
        add_mv(f0, r0);
        if (dorot)
          rotate(cube, f0, r0);
        mv ++;
      }
    }
  }
}

void solve_one(byte &cube[], bool dorot) {
  mv_n = 0;

  idx_ic = 24;
  idx_ie = 24;

  index_init();
  index_edge(cube, D, F);
  index_edge(cube, D, R);
  solve_phase(cube, mtb0, mtd0);

  index_init();
  index_corner(cube, D, F, R);
  index_edge(cube, F, R);
  solve_phase(cube, mtb1, mtd1);

  index_init();
  index_edge(cube, D, B);
  solve_phase(cube, mtb2, mtd2);

  index_init();
  index_corner(cube, D, R, B);
  index_edge(cube, R, B);
  solve_phase(cube, mtb3, mtd3);

  index_init();
  index_edge(cube, D, L);
  solve_phase(cube, mtb4, mtd4);

  index_init();
  index_corner(cube, D, B, L);
  index_edge(cube, B, L);
  solve_phase(cube, mtb5, mtd5);

  index_init();
  index_corner(cube, D, L, F);
  index_edge(cube, L, F);
  solve_phase(cube, mtb6, mtd6);

  index_init();
  index_corner(cube, U, R, F);
  index_corner(cube, U, F, L);
  index_corner(cube, U, L, B);
  solve_phase(cube, mtb7, mtd7);

  index_init();
  index_edge(cube, U, R);
  index_edge(cube, U, F);
  index_edge(cube, U, L);
  index_last();
  solve_phase(cube, mtb8, mtd8, dorot);
}

#define MAP(UU, FF) (imap[((UU)*NFACE)+(FF)]*NFACE)

const byte imap[] = {
  /*       U   F   D   B   R   L
  /* U */ -1,  0, -1,  1,  2,  3,
  /* F */  4, -1,  5, -1,  6,  7,
  /* D */ -1,  8, -1,  9, 10, 11,
  /* B */ 12, -1, 13, -1, 14, 15,
  /* R */ 16, 17, 18, 19, -1, -1,
  /* L */ 20, 21, 22, 23, -1, -1
};

const byte fmap[] = {
  /* -- */
  /* UF */ U, F, D, B, R, L,
  /* -- */
  /* UB */ U, B, D, F, L, R,
  /* UR */ U, R, D, L, B, F,
  /* UL */ U, L, D, R, F, B,

  /* FU */ F, U, B, D, L, R,
  /* -- */
  /* FD */ F, D, B, U, R, L,
  /* -- */
  /* FR */ F, R, B, L, U, D,
  /* FL */ F, L, B, R, D, U,

  /* -- */
  /* DF */ D, F, U, B, L, R,
  /* -- */
  /* DB */ D, B, U, F, R, L,
  /* DR */ D, R, U, L, F, B,
  /* DL */ D, L, U, R, B, F,

  /* BU */ B, U, F, D, R, L,
  /* -- */
  /* BD */ B, D, F, U, L, R,
  /* -- */
  /* BR */ B, R, F, L, D, U,
  /* BL */ B, L, F, R, U, D,

  /* RU */ R, U, L, D, F, B,
  /* RF */ R, F, L, B, D, U,
  /* RD */ R, D, L, U, B, F,
  /* RB */ R, B, L, F, U, D,
  /* -- */
  /* -- */

  /* LU */ L, U, R, D, B, F,
  /* LF */ L, F, R, B, U, D,
  /* LD */ L, D, R, U, F, B,
  /* LB */ L, B, R, F, D, U
  /* -- */
  /* -- */
};

const byte omap[] = {
  /* -- */
  /* UF */ 0, 0, 0, 0, 0, 0,
  /* -- */
  /* UB */ 4, 4, 4, 4, 0, 0,
  /* UR */ 6, 0, 2, 4, 4, 0,
  /* UL */ 2, 0, 6, 4, 0, 4,

  /* FU */ 4, 4, 4, 4, 2, 6,
  /* -- */
  /* FD */ 0, 0, 0, 0, 6, 2,
  /* -- */
  /* FR */ 6, 6, 2, 6, 4, 0,
  /* FL */ 2, 2, 6, 2, 0, 4,

  /* -- */
  /* DF */ 4, 4, 4, 4, 4, 4,
  /* -- */
  /* DB */ 0, 0, 0, 0, 4, 4,
  /* DR */ 6, 4, 2, 0, 4, 0,
  /* DL */ 2, 4, 6, 0, 0, 4,

  /* BU */ 0, 0, 0, 0, 2, 6,
  /* -- */
  /* BD */ 4, 4, 4, 4, 6, 2,
  /* -- */
  /* BR */ 6, 2, 2, 2, 4, 0,
  /* BL */ 2, 6, 6, 6, 0, 4,

  /* RU */ 4, 2, 0, 6, 2, 2,
  /* RF */ 2, 2, 2, 6, 2, 2,
  /* RD */ 0, 2, 4, 6, 2, 2,
  /* RB */ 6, 2, 6, 6, 2, 2,
  /* -- */
  /* -- */

  /* LU */ 4, 6, 0, 2, 6, 6,
  /* LF */ 6, 6, 6, 2, 6, 6,
  /* LD */ 0, 6, 4, 2, 6, 6,
  /* LB */ 2, 6, 2, 2, 6, 6,
  /* -- */
  /* -- */
};

int  solve_n;
byte solve_fce[MV_MAX];
int  solve_rot[MV_MAX];
byte solve_cube[NFACE*8];

#ifdef SOLVE_PATTERN

void apply_pattern(byte &cube[], byte &pat_fce[], char &pat_rot[]) {
  // Add moves to transform solved cube into pattern
  for (int i = 0; i < ArrayLen(pat_fce); i++) {
    solve_fce[solve_n] = pat_fce[i];
    solve_rot[solve_n] = pat_rot[i];
    solve_n++;
  }

  // Apply moves in reverse to a solved position and
  // replace the scanned cube with position that if
  // "solved" will result in the pattern
  copy_cube(cube, solve_cube);
  for (int i = solve_n-1; i >= 0; i--)
    rotate(cube, solve_fce[i], -solve_rot[i]);
}

#endif

bool solve_nomap(byte &cube[]) {
  solve_n = 0;
  copy_cube(solve_cube, cube);
  solve_one(solve_cube, true);

  if (!solved(solve_cube))
    return false;

  // Record the solution
  solve_n = mv_n;
  for (int i = 0; i < mv_n; i++) {
    solve_fce[i] = mv_f[i];
    solve_rot[i] = mv_r[i];
  }

#ifdef SOLVE_PATTERN
  if (
#ifdef ONLY_PATTERNS
      // Only create patterns and never a normal solve
      true
#else
      // If the cube is solved, always create a pattern
      solve_n == 0 ||
      // Otherwise decide at random to occasionally create a pattern
      // rather than a normal solve
      Random(6) == 0
#endif
      ) {
    int pat = -1;
    while (pat < 0) {
      // Select one of the included patterns at random
      // (keep looping until one is selected and applied)
      pat = Random(5);
      switch (pat) {
#ifdef PATTERN_CHECKERBOARD
      case 0:
        apply_pattern(cube, checkerboard_fce, checkerboard_rot);
        break;
#endif
#ifdef PATTERN_SPOT
      case 1:
        apply_pattern(cube, spot_fce, spot_rot);
        break;
#endif
#ifdef PATTERN_SNAKE
      case 2:
        apply_pattern(cube, snake_fce, snake_rot);
        break;
#endif
#ifdef PATTERN_CUBES
      case 3:
        apply_pattern(cube, cubes_fce, cubes_rot);
        break;
#endif
#ifdef PATTERN_SUPERFLIP
      case 4:
        apply_pattern(cube, superflip_fce, superflip_rot);
        break;
#endif
      default:
        // Indicate that no pattern has been applied
        pat = -1;
        break;
      }
    }
  }
#endif

  return true;
}

byte pmap[NFACE];

void solve_remap(byte &cube[], int map) {
  for (int f = 0; f < NFACE; f++)
    pmap[fmap[map+f]] = f;

  for (int f = 0; f < NFACE; f++) {
    int fs = fmap[map+f]*8;
    int fd = f*8;
    int os = omap[map+f];
    for (int od = 0; od < 8; od++) {
      solve_cube[fd+od] = pmap[cube[fs+os]];
      os = (os+1)&7;
    }
  }

  solve_one(solve_cube, false);
  if (mv_n < solve_n) {
    solve_n = mv_n;
    for (int i = 0; i < mv_n; i++) {
      solve_fce[i] = fmap[map+mv_f[i]];
      solve_rot[i] = mv_r[i];
    }
  }
}

bool solve(byte &cube[]) {
  if (!solve_nomap(cube))
    return false;

  flash_blue();

  if (solve_n <= TARGET)
    return true;

  for (int i = 1; i < NFACE*4; i++) {
    solve_remap(cube, i*NFACE);
    if (solve_n <= TARGET)
      return true;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Routines for color space conversion and color sorting
//-----------------------------------------------------------------------------

#define NFACE       6                // number of faces in cube
#define POS(FF, OO) (((FF)*8)+(OO))

const long cmax = 1024;

int  sc_r[NFACE*9];
int  sc_g[NFACE*9];
int  sc_b[NFACE*9];
int  sc_h[NFACE*9];
int  sc_s[NFACE*9];
int  sc_l[NFACE*9];
int  sc_sl[NFACE*9];
char sc_clr[NFACE*9];

void set_rgb(int o, int &rgb[]) {
  long r = rgb[0];
  long g = rgb[1];
  long b = rgb[2];
  // Convert to hsl
  long h = 0;
  long s = 0;
  long l = 0;
  long sl = 0;
  long v = r; if (g > v) v = g; if (b > v) v = b;
  long m = r; if (g < m) m = g; if (b < m) m = b;
  l = (v+m)/2;
  if (l > 0) {
    long vm = v-m;
    s = vm;
    if (s > 0) {
      long vf = v+m;
      if (l <= (cmax/2))
        vf = (2*cmax)-vf;
      long r2 = (v-r)*vf/vm;
      long g2 = (v-g)*vf/vm;
      long b2 = (v-b)*vf/vm;
      if (r == v)
        h = (g == m) ? 5*cmax+b2 : 1*cmax-g2;
      else if (g == v)
        h = (b == m) ? 1*cmax+r2 : 3*cmax-b2;
      else
        h = (r == m) ? 3*cmax+g2 : 5*cmax-r2;
      h += cmax; // rotate so R/B either side of 0
      h /= 6;
      if (h < 0)     h += cmax;
      if (h >= cmax) h -= cmax;
      sl = s*cmax/l;
    }
  }
  sc_r[o]  = r;
  sc_g[o]  = g;
  sc_b[o]  = b;
  sc_h[o]  = h;
  sc_s[o]  = s;
  sc_l[o]  = l;
  sc_sl[o] = sl;
  log(r);
  log(g);
  log(b);
  log(h);
  log(s);
  log(l);
  log(sl);
  logeol();
}

int clr_ratio(long c0, long c1) {
  int ratio = 0;
  if (c0 < c1)
    ratio = -(2000*(c1-c0)/(c1+c0));
  else if (c0 > c1)
    ratio =  (2000*(c0-c1)/(c1+c0));
  return ratio;
}

#define CMP_H    0
#define CMP_S    1
#define CMP_SL   2
#define CMP_SLR  3
#define CMP_L    4
#define CMP_LR   5
#define CMP_R_G  6
#define CMP_R_B  7
#define CMP_B_G  8

bool compare_clrs(const int cmp_fn, const int c0, const int c1) {
  bool cmp = false;
  switch (cmp_fn) {
  case CMP_H:   cmp = (sc_h[c1]  > sc_h[c0]);  break;
  case CMP_S:   cmp = (sc_s[c1]  > sc_s[c0]);  break;
  case CMP_SL:  cmp = (sc_sl[c1] > sc_sl[c0]); break;
  case CMP_SLR: cmp = (sc_sl[c1] < sc_sl[c0]); break;
  case CMP_L:   cmp = (sc_l[c1]  > sc_l[c0]);  break;
  case CMP_LR:  cmp = (sc_l[c1]  < sc_l[c0]);  break;
  case CMP_R_G: cmp = (clr_ratio(sc_r[c1], sc_g[c1])
                       < clr_ratio(sc_r[c0], sc_g[c0])); break;
  case CMP_R_B: cmp = (clr_ratio(sc_r[c1], sc_b[c1])
                       < clr_ratio(sc_r[c0], sc_b[c0])); break;
  case CMP_B_G: cmp = (clr_ratio(sc_b[c1], sc_g[c1])
                       < clr_ratio(sc_b[c0], sc_g[c0])); break;
  default:      error("cmp_fn", cmp_fn); break;
  }

  return cmp;
}

void sort_clrs(int &co[], const int s, const int n, const int cmp_fn) {
  const int e = s+n-2;
  int is = s;
  int ie = e;
  do {
    int il = e+2;
    int ih = s-2;
    for (int i = is; i <= ie; i++) {
      if (compare_clrs(cmp_fn, co[i+1], co[i])) {
        int o   = co[i];
        co[i]   = co[i+1];
        co[i+1] = o;
        if (i < il) il = i;
        if (i > ih) ih = i;
      }
    }
    is = il-1; if (is < s) is = s;
    ie = ih+1; if (ie > e) ie = e;
  } while (is <= ie);
}

void sort_colors(int &co[], const int t, const int s) {
#ifdef BLACK_FACE
  sort_clrs(co, 0*s, 6*s,
#ifdef WHITE_LOGO
	    (s == 1) ?
	    // Saturation
	    CMP_SL :
#endif
	    // Saturation (un-weighted by Lightness for black)
	    CMP_S);
#else
  // Lightness
  sort_clrs(co, 0*s, 6*s, CMP_LR);
  // Saturation
  sort_clrs(co, 0*s, 3*s, CMP_SL);
#endif // BLACK_FACE
  // Hue
  sort_clrs(co, 1*s, 5*s, CMP_H);
  // Red/Orange
  switch (t) {
  case 0: /* already sorted by hue */       break;
  case 1: sort_clrs(co, 1*s, 2*s, CMP_R_G); break;
  case 2: sort_clrs(co, 1*s, 2*s, CMP_B_G); break;
  case 3: sort_clrs(co, 1*s, 2*s, CMP_R_B); break;
  case 4: sort_clrs(co, 1*s, 2*s, CMP_SLR); break;
  case 5: sort_clrs(co, 1*s, 2*s, CMP_L);   break;
  }
  int i;
  for (i = 0; i < s; i++)
    sc_clr[co[i]] = CLR_W;
  for (; i < 2*s; i++)
    sc_clr[co[i]] = CLR_R;
  for (; i < 3*s; i++)
    sc_clr[co[i]] = CLR_O;
  for (; i < 4*s; i++)
    sc_clr[co[i]] = CLR_Y;
  for (; i < 5*s; i++)
    sc_clr[co[i]] = CLR_G;
  for (; i < 6*s; i++)
    sc_clr[co[i]] = CLR_B;
}

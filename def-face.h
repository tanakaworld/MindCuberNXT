
//-----------------------------------------------------------------------------
// Face indices
//-----------------------------------------------------------------------------

#define U        0
#define F        1
#define D        2
#define B        3
#define R        4
#define L        5

//-----------------------------------------------------------------------------
// Sequences of moves for patterns
//-----------------------------------------------------------------------------

#ifdef SOLVE_PATTERN

#ifdef PATTERN_CHECKERBOARD
const byte checkerboard_fce[] = { U, D, F, B, L, R};
const char checkerboard_rot[] = { 2, 2, 2, 2, 2, 2};
#endif

#ifdef PATTERN_SPOT
const byte spot_fce[] = { L, R, F, B, U, D, L, R};
const char spot_rot[] = {-1, 1, 1,-1, 1,-1,-1, 1};
#endif

#ifdef PATTERN_SNAKE
const byte snake_fce[] = { F, L, U, L, B, U, B, L, U, L, F, L, U, F};
const char snake_rot[] = {-1,-1, 1, 1, 2, 1, 2, 1,-1, 1,-1, 2,-1, 2};
#endif

#ifdef PATTERN_CUBES
const byte cubes_fce[] = { U, R, D, F, R, U, R, U, R, U, R, L, D, L, F, D, R};
const char cubes_rot[] = {-1, 1,-1,-1, 1, 2, 2,-1,-1, 1, 2, 1,-1,-1, 2, 2,-1};
#endif

#ifdef PATTERN_SUPERFLIP
const byte superflip_fce[] = { U, F, R, U, R, D, F, B, U, F, R, L, F, D, B, R, F, D, F, D};
const char superflip_rot[] = {-1, 2,-1, 2, 2, 2, 1,-1,-1, 2, 1, 1,-1, 2,-1, 2,-1, 2,-1,-1};
#endif

#endif

void banner() {
  ClearScreen();
#ifdef SOLVE_PATTERN
  TextOut(0, LCD_LINE1, "MindCuber v2.2p");
  TextOut(0, LCD_LINE2, "---------------");
#else
  TextOut(0, LCD_LINE1, "MindCuber v2.2");
  TextOut(0, LCD_LINE2, "--------------");
#endif
}

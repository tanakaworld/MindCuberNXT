#ifdef DEBUG

// Only include checks if DEBUG defined to save memory

void error(string mes, int num) {
  ClearLine(LCD_LINE1);
  TextOut(0, LCD_LINE1, "ERROR:");
  NumOut(8*6, LCD_LINE1, num);
  ClearLine(LCD_LINE2);
  TextOut(0, LCD_LINE2, mes);
  while (true)
    Wait(1);
}

#else

#define error(MES, NUM)

#endif

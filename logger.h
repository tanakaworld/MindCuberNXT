#ifdef LOG

string       logname = "mc.log";
byte         logh;
bool         logf = true;

void log(long n) {
  if (!logf)
    Write(logh, ',');
  if (n < 0) {
    Write(logh, '-');
    n = -n;
  }
  long f = 10;
  while (n >= f)
    f *= 10;
  while (f > 1) {
    f /= 10;
    byte b = (n/f)%10+'0';
    Write(logh, b);
  }
  logf = false;
}

void logeol() {
  byte eol = 10;
  Write(logh, eol);
  logf = true;
}

#define create_log()  DeleteFile(logname); CreateFile(logname, 18000, logh)
#define close_log()   CloseFile(logh)

#else

#define log(NUM)
#define logeol()
#define create_log()
#define close_log()

#endif

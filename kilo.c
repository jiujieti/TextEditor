#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// read original terminal ios
struct termios orig_termios;

// disable raw mode
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// enable raw mode
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);

  // disable raw mode when the program exits, either by main or exit()  
  atexit(disableRawMode);

  struct termios raw = orig_termios;
 
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();

  while(1) {
    char c;
    read(STDIN_FILENO, &c, 1);
    if(iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if(c == 'q') break;
  }

  return 0;
}

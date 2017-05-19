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
  // disable ctrl+s and ctrs+q for stop and resume transmission
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

  raw.c_oflag &= ~(OPOST);

  raw.c_cflag |= (CS8);

  // char by char reading, diasble ctrl+z and ctrl+c
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  
  char c;
  while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if(iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
  }
  return 0;
}
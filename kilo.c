/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)


/*** data ***/

/* read original terminal ios */
struct termios orig_termios;

/*** terminal ***/

/* error handling */
void die(const char *s) {
  perror(s);
  exit(1);
}

/* disable raw mode */
void disableRawMode() {
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

/* enable raw mode */
void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

  // disable raw mode when the program exits, either by main or exit()  
  atexit(disableRawMode);

  struct termios raw = orig_termios;
 
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey() {
  int nread;
  char c;
  while((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if(nread == -1 && errno != EAGAIN) die("read");    
  }
  return c;
}

/*** output ***/

/* clear the entire screen by using 2J */
void editorRefreshScreen() {
  // escape sequences starting wth \x1b instructs terminal to do text formatting tasks
  write(STDOUT_FILENO, "\x1b[2J", 4);
}

/*** input ***/

void editorProcessKeypress() {
  char c = editorReadKey();

  switch(c) {
    case(CTRL_KEY('q')):
      exit(0);
      break;
  }
}

/*** init ***/

int main() {
  enableRawMode();

  while(1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
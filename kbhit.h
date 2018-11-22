#ifndef _KBHIT_H_
#define _KBHIT_H_

#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

extern void KB_open(void);
extern void KB_close(void);
extern bool kbhit(void);
extern char linux_getch(void);


#endif /* _KBHIT_H_ */

#include "kbhit.h"

static struct termios Old_set;
static struct termios New_set;
static int ReadChar = -1;

void KB_open()
{
    tcgetattr(0,&Old_set);
    New_set = Old_set;
    New_set.c_lflag &= ~ICANON;
    New_set.c_lflag &= ~ECHO;
    New_set.c_lflag &= ~ISIG;
    New_set.c_cc[VMIN] = 0;
    New_set.c_cc[VTIME] = 0;
    tcsetattr(0,TCSANOW,&Old_set);
}

void KB_close()
{
    tcsetattr(0,TCSANOW, &Old_set);
}

bool kbhit()
{
    char ch;
    int nread;

    if(ReadChar !=-1) {
        return true;
    }

    New_set.c_cc[VMIN]=0;
    tcsetattr(0,TCSANOW,&New_set);
    nread=read(0,&ch,1);
    New_set.c_cc[VMIN]=1;
    tcsetattr(0,TCSANOW,&New_set);

    if(nread == 1) {
        ReadChar = ch;
        return true;
    }

    return false;
}

char linux_getch()
{
    char ch;

    if(ReadChar != -1) {
       ch = ReadChar;
       ReadChar = -1;
       return (ch);
    }

    read(0,&ch,1);
    return(ch);
}

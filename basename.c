#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Author: Aaron Cox
 */

//TODO: add functionality for . and ..
static char* basename(char* path)
{
  char *p, *ret;
  int len = strlen(path);
  p = path + len - 1;

  //first eat trailing /'s
  while( *p == '/' && p != path) {
    *p = '\0';
    p--;
  }
  if( p == path ) {
    ret = malloc(2);
    ret[0] = *p == '/' ? '/' : *p;
    ret[1] = '\0';
    return ret;
  }

  // then, find the first directory division
  while( *p != '/' && p != path ) {
    p--;
  }
  if( *p == '/' ) {
    ret = malloc(strlen(p+1)+1);
    strcpy(ret,p+1);
  }
  else {
    ret = malloc(strlen(p)+1);
    strcpy(ret, p);
  }
  return ret;
}

int main(int argc, char *argv[])
{
  printf("%s\n", basename(argv[1]));
}

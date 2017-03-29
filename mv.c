/* Author: Aaron Cox
 */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void mv_usage(void)
{
  printf(2, "mv_usage: mv <file> <newfile>\n");
  printf(2, "          mv <file1> [<file2> ...] <directory>\n");
}

static char* dirname(char* path)
{
  char *dirn;
  int len = strlen(path);
  int i;


  while( path[len-1] == '/' && len > 1 ) {
    len--;
  }
  if( len == 1 ) {
    dirn = malloc(2);
    dirn[0] = path[0] == '/' ? '/' : '.';
    dirn[1] = '\0';
    return dirn;
  }
  else if( len == 2 && path[0] == '.' && path[1] == '.' ) {
    dirn = malloc(3);
    dirn[0] = '.'; dirn[1] = '.'; dirn[2] = '\0';
    return dirn;
  }

  while( path[len-1] != '/' && len > 1 ) {
    len--;
  }
  if( len == 1 ) {
    dirn = malloc(2);
    dirn[0] = path[0] == '/' ? '/' : '.';
    dirn[1] = '\0';
  }
  else {
    dirn = malloc(len + 1);
    for( i = 0; i < len - 1; i++ ) {
      dirn[i] = path[i];
    }
  }
  return dirn;
}

//TODO: add functionality for . and ..
static char* basename(char* path)
{
  char *p, *ret;
  int len = strlen(path);
  p = path + len - 1;

  //first eat trailing /'s
  while( *p == '/' && p != path) {
    p--;
  }
  if( p == path ) {
    ret = malloc(2);
    ret[0] = '/'; ret[1] = '\0';
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

static char* build_path(char *dir, char *fn)
{
  int dirlen, baselen;
  char *newpath, *base;
  
  base = basename(fn);
  dirlen  = strlen(dir);
  baselen = strlen(base);

  newpath = malloc( dirlen + baselen + 2 );
  strcpy(newpath, dir);
  newpath[dirlen++] = '/';
  strcpy(newpath+dirlen, base);
  free(base);
  return newpath;
}

static int mv(char *old, char *new)
{
  struct stat ost, nst;
  int lennew = strlen(new), lenold;
  int ret;
  char *newpath, *dirn;


  // first, make sure what we're moving actually exists
  ret = stat(old, &ost);
  if( ret != 0 ) {
    printf(2, "mv: cannot stat %s. No file or directory exists.\n", old);
    return ret;
  }

  // remove all trailing /'s from the destination name
  while( lennew > 1 && new[lennew-1] == '/' ) {
    new[--lennew] = '\0';
  }

  ret = stat(new, &nst);
  if( ost.type == T_FILE ) {
    if( ret == 0 ) {
      // case1 : mv file file
      if( nst.type == T_FILE ) {
        ret = rename(old, new);
      }
      // case2 : mv file dir
      else if( nst.type == T_DIR ) {
        newpath = build_path(new, old);
        ret = rename(old, newpath);
        free(newpath);
      }
      else {
        printf(2,"mv: can only mv regular files and directories\n");
        exit();
      }
    }
    else {
      // case3 : mv file newfile
      dirn = dirname(new);
      if( stat(dirn, &nst) != 0 ) {
        printf(2,"mv: directory %s doesn't exist\n", dirn);
        exit();
      }
      ret = rename(old, new);
      free(dirn);
    }
  }


  else if( ost.type == T_DIR ) {
    if( ret == 0 ) {
      if( nst.type == T_DIR ) {
        // case4 : mv dir dir
        ret = rename(old, new);
        if( ret != 0 ) {
          
      }
      else {
        // case5 : mv dir file,dev
        printf(2,"mv: can't overwrite non-directory '%s' with directory '%s'\n", new, old);
        exit();
      }
    }
    else {
      // case6 : mv dir newdir
    }
  }
  else {
    printf(2, "mv: can only mv regular files and directories.\n");
    return -1;
  }

  return ret;
}

int main(int argc, char *argv[])
{
  rename(argv[1], argv[2]);
  exit();
}

/*
int main( int argc, char *argv[])
{
  struct stat dest;
  int i,ret;

  // if argc < 3, not enough arguments
  if( argc < 3 ) {
    mv_usage();
    exit();
  }

  // if argc == 3, perform 1 mv
  else if( argc == 3 ) {
    ret = mv(argv[1], argv[2]);
    exit();
  }
   
  // if argc > 3, make sure last arg is directory and exists
  // and then perform moves for each intermediate arg
  ret = stat(argv[argc-1], &dest);
  if( ret == 0 && dest.type != T_DIR ) {
    mv_usage();
    exit();
  }
  else if( ret != 0 ) {
    printf(2, "mv: Directory %s does not exist.\n", argv[argc-1]);
    exit();
  }

  //lendest = strlen(argv[argc-1]);
  for( i = 1; i < argc-1; i++ ) {
    ret = mv(argv[i], argv[argc-1]);
  }

  exit();
}*/

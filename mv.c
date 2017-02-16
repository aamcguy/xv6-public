
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void mv_usage(void)
{
  printf(2, "mv_usage: mv <file> <newfile>\n");
  printf(2, "          mv <file1> [<file2> ...] <directory>\n");
}

// double //'s will kill this fx
char* basename(char* path)
{
  char *p;
  int len = strlen(path);
  p = path + len - 1;
  while( *p == '/' && p != path) {
    p--;
  }
  if( p == path) {
    return "/";
  }

  while( *p != '/' && p != path ) {
    p--;
  }
  return *p == '/' ? p+1 : path;
}

int mv_file(char *old, char *new)
{
  //struct stat ost;
  struct stat nst;
  char *fullpath, *base;
  int lennew = strlen(new);
  //int lenold = strlen(old);
  int ret;

  ret = stat(new, &nst);
  if( ret == 0 ) {
    if( nst.type == T_DIR) {
      base = basename(old);
      int lenbase = strlen(base);
      fullpath = malloc( (strlen(base)+strlen(new)+2) * sizeof(char));
      strcpy(fullpath, new);
      if( lennew == 1 && *new == '/' ) {
        strcpy(fullpath+lennew, base);
        fullpath[lennew+lenbase] = '\0';
      }
      else {
        fullpath[lennew] = '/';
        strcpy(fullpath+lennew+1, base);
        fullpath[lennew+lenbase+1] = '\0';
      }
      ret = link(old, fullpath);
      if( ret != 0 ) {
        printf(2, "mv: error on link when dest is T_DIR\n");
        return ret;
      }
      ret = unlink(old);
      if( ret != 0 ) {
        printf(2, "mv: error on unlink when dest is T_DIR\n");
        return ret;
      }
    }
    else if(nst.type == T_FILE ) {
      ret = unlink(new);
      if( ret != 0 ) {
        printf(2, "mv_file: error on first unlink when dest is T_FILE\n");
        return ret;
      }
      ret = link(old, new);
      if( ret != 0 ) {
        printf(2, "mv_file: error on link when dest is T_FILE\n");
        return ret;
      }
      ret = unlink(old);
      if( ret != 0 ) {
        printf(2, "mv_file: error on second unlink when dest is T_FILE\n");
        return ret;
      }
    }
    else {
      printf(2, "error: can only overwrite regular files and directories with mv.\n");
      return -1;
    }
  }
  // else we need to verify that directory location is good and create new file
  // TODO: actually do this else statement right...
  else {
    //printf(2,"mv_file: cannot yet just create
    ret = link(old, new);
    if( ret != 0 ) {
      printf(2, "mv_file: error on link when dest is T_FILE that doesn't exist yet\n");
      return ret;
    }
    ret = unlink(old);
    if( ret != 0 ) {
      printf(2, "mv_file: error on second unlink when dest is T_FILE\n");
      return ret;
    }
  }
  return 0;
}

int mv_dir(char *old, char *new)
{
  printf(2,"mv cannot yet move/rename directories\n");
  return -1;
}

int mv(char *old, char *new)
{
  struct stat ost;
  //struct stat nst;
  int lennew = strlen(new);
  int ret;

  while( lennew > 1 && new[lennew-1] == '/' ) {
    new[lennew-- - 1] = '\0';
  }

  ret = stat(old, &ost);
  if( ret != 0 ) {
    printf(2, "mv: cannot stat %s. No file or directory exists.\n", old);
    return ret;
  }

  //ret = stat(new, &nst);
  if( ost.type == T_FILE ) {
    ret = mv_file(old, new);
  }
  else if( ost.type == T_DIR ) {
    ret = mv_dir(old, new);
  }
  else {
    printf(2, "error: can only mv regular files and directories.\n");
    return -1;
  }
  
  return ret;
}

int main( int argc, char *argv[])
{
  struct stat dest;
  int i,ret;

  if( argc < 3 ) {
    mv_usage();
    exit();
  }
  else if( argc == 3 ) {
    ret = mv(argv[1], argv[2]);
    if( ret != 0 ) {
      return ret;
    }
    exit();
  }
    
  ret = stat(argv[argc-1], &dest);
  if( ret == 0 && dest.type != T_DIR ) {
    mv_usage();
    return -1;
  }
  else if( ret != 0 ) {
    printf(2, "mv: Directory %s does not exist.\n", argv[argc-1]);
    return -1;
  }
  for( i = 1; i < argc-1; i++ ) {
    ret = mv(argv[i], argv[argc-1]);
  }

  exit();
}

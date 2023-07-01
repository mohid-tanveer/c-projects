#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#else
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "lib/map.h"

void my_itoa(char *buff, int len, unsigned int n) { snprintf(buff, len, "%x", n); }

unsigned int unique_int_key(map_t *map) {
  // create random token
  static char buff[64];
  unsigned int tok;
  do {
    tok = rand();
    if (!tok) {
      tok++;
    }
    my_itoa(buff, 64, tok);
  } while (map_get(map, buff, NULL));
  return tok;
}

void cat_home_dir(char *buff, int len, char *fname) {
#ifdef _WIN32
    // Windows implementation
    char userProfile[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userProfile))) {
        snprintf(buff, len, "%s\\%s", userProfile, fname);
    }
#else
    // Unix-like implementation
    const char *homedir = getenv("HOME");
    if (homedir) {
        snprintf(buff, len, "%s/%s", homedir, fname);
    }
#endif
}

char *strip(char *str) {
  while (isspace(*str))
    str++;
  if (!*str) {
    return str;
  }
  char *e = str + strlen(str) - 1;
  while (e > str && isspace(*e)) {
    e--;
  }
  *(e + 1) = '\0';
  return str;
}

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

void itoa(char *buff, int len, unsigned int n) { snprintf(buff, len, "%x", n); }

unsigned int unique_int_key(map_t *map) {
  // create random token
  static char buff[64];
  unsigned int tok;
  do {
    tok = rand();
    if (!tok) {
      tok++;
    }
    itoa(buff, 64, tok);
  } while (map_get(map, buff, NULL));
  return tok;
}

void cat_home_dir(char *buff, int len, char *fname) {
#ifdef _WIN32
    PWSTR widePath;
    if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &widePath))) {
        char narrowPath[MAX_PATH];
        wcstombs_s(NULL, narrowPath, MAX_PATH, widePath, MAX_PATH);
        strcat_s(buff, len, narrowPath);
        strcat_s(buff, len, "\\");
        strcat_s(buff, len, fname);
        CoTaskMemFree(widePath);
    }
#else
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    strncat(buff, homedir, len);
    strncat(buff, "/", len - strlen(buff));
    strncat(buff, fname, len - strlen(buff));
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

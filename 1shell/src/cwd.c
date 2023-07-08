#ifdef _WIN32
  #include <direct.h>
#else
  #include <pwd.h>
  #include <unistd.h>
#endif

char *cwd() {
  static char buffer[256];
#ifdef _WIN32
  _getcwd(buffer, 256); // ignore size error.
#else
  getcwd(buffer, 256); // ignore size error.
#endif 
  return buffer;
}

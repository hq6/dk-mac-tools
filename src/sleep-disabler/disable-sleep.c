#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char** argv){
  char* execName = basename(argv[0]);
  if (argc == 2) {
    if (strcmp(argv[1], "--summary") == 0) {
      printf("Disable and enable sleep without root\n");
      return 0;
    } else if (strcmp(argv[1], "--help") == 0) {
      printf("USAGE\n"
          "   dk %s\n", execName);
      return 0;
    }
  }

  int newuid = geteuid();
  if (newuid != 0) {
    char cmd_buf[1024];
    sprintf(cmd_buf, "sudo bash -c 'chown root %s && chmod u+s %s'", argv[0], argv[0]);
    fprintf(stderr, "The binary is not setuid root.\n"
    "Attempting to setuid root and re-execute.\n\n\t%s\n\n", cmd_buf);
    system(cmd_buf);

    // Re-execute the same command
    execvp(argv[0], argv);
  }
  setuid(newuid);

  if (strcmp(execName, "disable-sleep") == 0) {
    fprintf(stderr, "pmset disablesleep 1\n");
    system("pmset disablesleep 1");
  } else if (strcmp(execName, "enable-sleep") == 0) {
    fprintf(stderr, "pmset disablesleep 0\n");
    system("pmset disablesleep 0");
  } else {
    fprintf(stderr, "Unsupported name '%s'. This binary must be named disable-sleep or"
        " enable-sleep.\n", execName);
  }
}

#include "user.h"

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};

int main() {
  struct stat st;
  int fd = open("/", 0);
  fstat(fd, &st);
  printf("type=%d ino=%d\n", st.type, st.ino);
  close(fd);
  exit(0);
}
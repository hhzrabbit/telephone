#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>

int main(){
  int key = ftok("README.md", 22);
  int semid;
  int shmid;
  int * shm;
  int amtToRead;
  int sc;
  int fd;

  shmid = shmget(key, 4, 0644);
  shm = shmat(shmid, 0, 0);
  amtToRead = * shm;

  fd = open("story.txt", O_RDONLY, 0644);

  char buf[amtToRead + 1];
  
  read(fd, &buf, amtToRead);
  buf[amtToRead] = 0;
  
  printf("%s\n", buf);
  close(fd);

  printf("\n\nEnter next line: ");
  
  char d[1024];
  char * dest = d;
  fgets(dest, 256, stdin);

  
  printf("len: %ld\n", strlen(d));
  
  return 0;
}

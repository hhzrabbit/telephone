#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "semOps.h"

void printLastLine(int * shm){
  int fd;
  int amtToRead = * shm;
  char buf[amtToRead + 1];
  
  fd = open("story.txt", O_RDONLY, 0644);
  lseek(fd, (-1 * amtToRead), SEEK_END);
  
  read(fd, &buf, amtToRead);
  buf[amtToRead] = 0;

  printf("Last line of story: %s\n", buf);
  close(fd);

}

void getAndSaveNextLine(int * shm){
  char d[1024];
  char * dest = d;
  int fd;
  int lenInput;

  printf("Enter next line: ");
  fgets(dest, 1024, stdin);
  
  lenInput = strlen(d);
  * shm = lenInput;
  
  fd = open("story.txt", O_WRONLY | O_APPEND, 0644);
  
  write(fd, d, lenInput);
  close(fd);

  printf("Line added: %s\n", d);

}

int main(){
  int key = ftok("README.md", 22);
  int semid;
  int shmid;
  int * shm;
  int sc;

  semid = semget(key, 1,  0644);
  if (semid == -1)
    printf("game not in session. type ./control -c to start a game.\n");
  else {
    semDown(semid);
    
    shmid = shmget(key, 4, 0644);
    shm = shmat(shmid, 0, 0);
    
    printLastLine(shm);
    
    getAndSaveNextLine(shm);
  
    semUp(semid);
  }
  return 0;
}

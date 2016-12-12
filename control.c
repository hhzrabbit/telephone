#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "semOps.h"

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};

int sizeOfStory(){
  struct stat fileInfo;
  stat("story.txt", &fileInfo);
  return fileInfo.st_size;
}

void clearStory(){
  int fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
  close(fd);
}

void printStory(){
  int amt = sizeOfStory();
  char buf[amt + 1];
  
  int fd = open("story.txt", O_RDONLY, 0644);
  read(fd, &buf, sizeOfStory());
  buf[amt] = 0;
  
  printf("Story:\n%s\n", buf);
  close(fd);
}

void setup(int key){
  int semid;
  int shmid;
  int * shm;
  int sc;
  int fd;
  union semun su;

  semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);

  if (semid == -1)
    printf("game already in session.\n");

  else {
    printf("semaphore created, id %d\n", semid);
    su.val = 1;
    
    //setting semaphore value
    sc = semctl(semid, 0, SETVAL, su);
    printf("semaphore value set: %d\n", su.val);

    semDown(semid);
    
    //setting shared memory value
    shmid = shmget(key, 4, IPC_CREAT | 0644);
    printf("shared memory created, id %d\n", shmid);
    shm = shmat(shmid, 0, 0);
    * shm = 0;

    semUp(semid);
    
    printf("shared memory value set: %d\n", * shm);

    //make text file
    clearStory();
    
  }
}

void end(int key){
  int semid;
  int shmid;
  int sc;

  semid = semget(key, 1, 0644);
  
  printStory();
  clearStory();

  semDown(semid);
  //remove shared memory
  shmid = shmget(key, 4, 0644);
  shmctl(shmid, IPC_RMID, 0);
  printf("shared memory removed\n");
  
  //removing a semaphore
  sc = semctl(semid, 0, IPC_RMID);
  printf("semaphore removed\n");
    
}

int main(int argc, char *argv[]){
  int key = ftok("README.md", 22);

  umask(0);
  
  if (strncmp(argv[1], "-c", strlen(argv[1])) == 0)
    setup(key);
  
  else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0)
    printStory();
  
  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0)
    end(key);
  
  else 
    printf("No such flag. Use -c for create, -v for view, or -r for remove.\n");
  
  return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};

int main(int argc, char *argv[]){
  int key = ftok("README.md", 22);
  int semid;
  int shmid;
  int * shm;
  int sc;
  int fd;
  
  if (strncmp(argv[1], "-c", strlen(argv[1])) == 0){
    semid = semget(key, 1, IPC_CREAT | 0644);
    printf("semaphore created, id %d\n", semid);
    union semun su;
    su.val = 1;
    //setting semaphore value
    sc = semctl(semid, 0, SETVAL, su);
    printf("semaphore value set: %d\n", sc);

    shmid = shmget(key, 4, IPC_CREAT | 0644);
    printf("shared memory created, id %d\n", shmid);
    shm = shmat(shmid, 0, 0);
    * shm = 0;
    
    printf("shared memory value set: %d\n", * shm);

    fd = open("story.txt", O_CREAT | O_APPEND, 0644);
    close(fd);
    printf("file created, fd %d\n", fd);
  }
  else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0){
    semid = semget(key, 1, 0);
    //getting the value of a semaphore
    sc = semctl(semid, 0, GETVAL);

    printf("semaphore value: %d\n",sc);
  }
  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
    semid = semget(key, 1, 0);
    //removing a semaphore
    sc = semctl(semid, 0, IPC_RMID);
    printf("semaphore removed: %d\n", sc);
  }
  return 0;

}

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
  int totBytesId;
  int * totBytes;
  int sc;
  int fd;

  umask(0);
  
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

    totBytesId = shmget(key + 20, 4, IPC_CREAT | 0644);
    totBytes = shmat(totBytesId, 0, 0);
    * totBytes = 0;
    
    printf("shared memory value set: %d\n", * shm);

    fd = open("story.txt", O_CREAT | O_APPEND, 0644);
    close(fd);
    printf("file created, fd %d\n", fd);
  }
  else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0){
    int amt = * totBytes;
    char buf[amt + 1];
    
    totBytesId = shmget(key + 20, 4, IPC_CREAT | 0644);
    totBytes = shmat(totBytesId, 0, 0);
    
    fd = open("story.txt", O_RDONLY, 0644);
    read(fd, &buf, amt);
    buf[amt] = 0;

    printf("Story: %s\n", buf);
    close(fd);
  }
  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
    struct shmid_ds d;

    //remove shared memory
    shmid = shmget(key, 4, IPC_CREAT | 0644);
    shmctl(shmid, IPC_RMID, &d);
    
    semid = semget(key, 1, 0);
    //removing a semaphore
    sc = semctl(semid, 0, IPC_RMID);
    printf("semaphore removed: %d\n", sc);

    //print out the story
    int amt = * totBytes;
    char buf[amt + 1];

    totBytesId = shmget(key + 20, 4, IPC_CREAT | 0644);
    totBytes = shmat(totBytesId, 0, 0);
  
    fd = open("story.txt", O_RDONLY, 0644);
    read(fd, &buf, amt);
    buf[amt] = 0;
    
    printf("Story: %s\n", buf);
    close(fd);

    //clear story.txt
    fd = open("story.txt", O_WRONLY | O_TRUNC, 0644);
    write(fd, "", 0);
    close(fd);

    //remove shared memory that stores total amount of bytes
    totBytesId = shmget(key + 20, 4, IPC_CREAT | 0644);
    shmctl(totBytesId, IPC_RMID, &d);
    
  }
  return 0;

}
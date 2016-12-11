void semDown(int semid){
  struct sembuf operation;
  operation.sem_op = -1;
  operation.sem_num = 1;
  operation.sem_flg = SEM_UNDO;
  semop(semid, &operation, 1);
}

void semUp(int semid){
  struct sembuf operation;
  operation.sem_op = 1;
  operation.sem_num = 1;
  operation.sem_flg = SEM_UNDO;
  semop(semid, &operation, 1);
}

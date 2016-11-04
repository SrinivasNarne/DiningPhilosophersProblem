#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pthread.h"
#include "sem.h"
#include <sys/mman.h>
#include <fcntl.h>

int numberOfPhilosophers = 0;
int numberOfIterations = 0;
int philosopherStatus[100] = {0};
int philosopherNumber = 0;

void philosopher(int philosopherNumber);
void think(int philosopherNumber);
void take_fork(int philosopherNumber);
void eat(int philosopherNumber);
void put_fork(int philosopherNumber);
void test(int philosopherNumber);
int findLeft(int philosopherNumber);
int findRight(int philosopherNumber);
int *status_create(int length);
int *status_read(int index, int length);
int *status_write(int index, int length, int value);

int main(int argc, char *argv[]) {
  int i, pid;
  numberOfPhilosophers = atoi(argv[1]);
  numberOfIterations = atoi(argv[2]);
  philosopherNumber = atoi(argv[3]);



    char *str1 = "/tmp/sem";
    char str2[10];
    char str3[100];
    sprintf(str2, "%d", philosopherNumber);
    strcpy(str3, str1);
    strcat(str3, str2);

    semaphore_t *me2;
    //printf("Creating semaphore");
    me2 = semaphore_create(str3, 0);
    if (me2 == NULL)
      exit(1);
    semaphore_close(me2);



  for (i = 0; i < atoi(argv[2]); i ++) {
    philosopher(philosopherNumber);
  }
  return 0;
}

void philosopher(int philosopherNumber) {
  int i;
    think(philosopherNumber);
    take_fork(philosopherNumber);
    eat(philosopherNumber);
    put_fork(philosopherNumber);

}

void think(int philosopherNumber) {
  status_write(philosopherNumber, numberOfPhilosophers, 0);
  printf("Philosopher %d is thinking.\n", philosopherNumber);
  usleep(100);
}

void take_fork(int philosopherNumber) {
  //down(&m);
  semaphore_t *semap;
  semap = semaphore_open("/tmp/semaphore");
  if (semap == NULL)
    exit(1);
  semaphore_wait(semap);
  semaphore_close(semap);

  status_write(philosopherNumber, numberOfPhilosophers, 1);
  printf("Philosopher %d is hungry.\n", philosopherNumber);
  test(philosopherNumber);
  //up(&m);
  semaphore_t *semap3;
  semap3 = semaphore_open("/tmp/semaphore");
  if (semap3 == NULL)
    exit(1);
  semaphore_post(semap3);
  semaphore_close(semap3);

  //down(s[i]);
  semaphore_t *semap2;
  char *str1 = "/tmp/sem";
  char str2[10];
  char str3[100];
  sprintf(str2, "%d", philosopherNumber);
  strcpy(str3, str1);
  strcat(str3, str2);
  semap2 = semaphore_open(str3);
  if (semap2 == NULL)
    exit(1);
  semaphore_wait(semap2);
  semaphore_close(semap2);
}

void eat(int philosopherNumber) {
  status_write(philosopherNumber, numberOfPhilosophers, 2);
  usleep(100);
}

void put_fork(int philosopherNumber) {
  //down(&m);
  semaphore_t *semap;
  semap = semaphore_open("/tmp/semaphore");
  if (semap == NULL)
    exit(1);
  semaphore_wait(semap);
  semaphore_close(semap);

  status_write(philosopherNumber, numberOfPhilosophers, 0);
  printf("Philosopher %d is thinking.\n", philosopherNumber);
  test(findLeft(philosopherNumber));
  test(findRight(philosopherNumber));
  //up(&m);
  semaphore_t *semap2;
  semap2 = semaphore_open("/tmp/semaphore");
  if (semap2 == NULL)
    exit(1);
  semaphore_post(semap2);
  semaphore_close(semap2);
}

void test(int philosopherNumber) {
  int *ptr1 = status_read(philosopherNumber, numberOfPhilosophers);
  int *ptr2 = status_read(findLeft(philosopherNumber), numberOfPhilosophers);
  int *ptr3 = status_read(findRight(philosopherNumber), numberOfPhilosophers);
  if (((*ptr1) == 1) && ((*ptr2) != 2) && ((*ptr3) != 2)) {
    status_write(philosopherNumber, numberOfPhilosophers, 2);
    printf("Philosopher %d is eating.\n", philosopherNumber);
    //up(s[i]);
    semaphore_t *semap2;
    char *str1 = "/tmp/sem";
    char str2[10];
    char str3[100];
    sprintf(str2, "%d", philosopherNumber);
    strcpy(str3, str1);
    strcat(str3, str2);
    semap2 = semaphore_open(str3);
    if (semap2 == NULL)
      exit(1);
    semaphore_post(semap2);
    semaphore_close(semap2);
  }
}

int findLeft(int philosopherNumber) {
  if (philosopherNumber == 0) {
    return numberOfPhilosophers - 1;
  } else {
    return philosopherNumber - 1;
  }
}

int findRight (int philosopherNumber) {
  if (philosopherNumber == numberOfPhilosophers - 1) {
    return 0;
  } else {
    return philosopherNumber + 1;
  }
}

int *status_create(int length) {
  char *status_name = "tmp/status";
  int fd = open(status_name, O_RDWR | O_CREAT | O_EXCL, 0666);
  if (fd < 0) {
    return NULL;
  }
  (void) ftruncate(fd, length*sizeof(int));
  int *p = (int *)mmap(NULL, length*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  return p;
}

int *status_read(int index, int length) {
  char *status_name = "tmp/status";
  int fd = open(status_name, O_RDWR, 0666);
  if (fd < 0)
    return NULL;
  (void) ftruncate(fd, length*sizeof(int));
  int *p = (int *)mmap(NULL, length*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  return (p + index);
}

int *status_write(int index, int length, int value) {
  char *status_name = "tmp/status";
  int fd = open(status_name, O_RDWR, 0666);
  if (fd < 0)
    return NULL;
  (void) ftruncate(fd, length*sizeof(int));
  int *p = (int *)mmap(NULL, length*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  p[index] = value;
  return p;
}

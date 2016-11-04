#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "pthread.h"
#include "sem.h"
#include <sys/mman.h>
#include <fcntl.h>


int *status_create(int length);
int *status_read(int index, int length);
int *status_write(int index, int length, int value);

int numberOfPhilosophers = 0;
int numberOfIterations = 0;

int main(int argc, char *argv[]) {
  char *file[] = {"./philosopher", argv[1], argv[2], NULL};
  numberOfPhilosophers = atoi(argv[1]);
  numberOfIterations = atoi(argv[2]);
  int i;
  int pid=0;
  semaphore_t *me;
  me = semaphore_create("/tmp/semaphore", 1);
  if (me == NULL) {
    printf("Exiting");
    exit(1);
  }
  semaphore_close(me);
  status_create(numberOfPhilosophers);
  for (i = 0; i < numberOfPhilosophers; i ++) {
    pid = fork();
    if (pid==0){
      char str[10];
      sprintf(str, "%d", i);
      file[3] = str;
      file[4] = NULL;
      execvp(file[0], file);
    }

  }
  for (i = 0; i < numberOfPhilosophers; i ++) {
    wait(NULL);
  }

  return 0;
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

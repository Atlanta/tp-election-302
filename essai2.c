#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int n = 3;
  int k = 0;
  int* tube = (int*) malloc(2*n*sizeof(int));

  for (int i = 0; i < 2*n; i = i+2) {
      pipe(&tube[i]);
  }

  for (int i = 0; i < n; i++) {
      if (fork() == 0) {
          int in;
          if (i != 0)
              in = (2*(i-1))%(2*n);
          else
              in = (2*(i))%(2*n);
          int out = (2*i) + 1;
          read(tube[in], &k, sizeof(int));
          for (int j = 0; j < 2*n; j++) {
              if (!(j == in || j == out)) {
                  close(tube[j]);
              }
          }
          k++;
          printf("%d ", k);
          //write(tube[out], &c, sizeof(int));
          close(tube[in]);
          close(tube[out]);
          printf("\n");
          exit(0);
      }
  }
  for (int i = 0; i < n; i++) {
      wait(NULL);
  }
  exit(0);
}

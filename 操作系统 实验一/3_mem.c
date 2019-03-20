#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
int *p = malloc(sizeof(int)); // a1
assert(p != NULL);
printf("(%d) address pointed to by p: %p\n",
getpid(), p); // a2
*p = 0; // a3
while (1) {
for(int i=0;i<100000;i++){
for(int j=0;j<500;j++){
}}
*p = *p + 1;
printf("(%d) p: %d\n", getpid(), *p); // a4
}
 return 0;
}


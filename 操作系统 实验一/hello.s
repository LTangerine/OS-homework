.section .data 
string:
      .ascii "hello world!\n"
.section .text
.globl _start
_start:
mov $4,%eax
mov $1,%ebx
mov $string,%ecx
mov $13,%edx
int $0x80
mov $1,%eax
mov $0,%ebx
int $0x80

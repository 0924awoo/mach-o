.file "test1.s"
.text
.globl _main
_main:
    movq $0x2000001, %rax
    movq hoge(%rip), %rdi
    syscall
.data
hoge:
    .byte 0x5

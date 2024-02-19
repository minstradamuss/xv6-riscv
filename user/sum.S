isNumericChar:
        push    rbp
        mov     rbp, rsp
        mov     eax, edi
        mov     BYTE PTR [rbp-4], al
        cmp     BYTE PTR [rbp-4], 0
        je      .L2
        cmp     BYTE PTR [rbp-4], 47
        jle     .L3
        cmp     BYTE PTR [rbp-4], 57
        jle     .L2
.L3:
        mov     eax, 1
        jmp     .L5
.L2:
        mov     eax, 0
.L5:
        pop     rbp
        ret
.LC0:
        .string "error: reached the end of the current file"
.LC1:
        .string "error: couldn't read"
.LC2:
        .string "Buffer overflow"
.LC3:
        .string "error: couldn't find the space"
.LC4:
        .string "error: couldn't find the second number"
.LC5:
        .string "error: couldn't parse the numbers"
.LC6:
        .string "%d\n"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 96
        mov     DWORD PTR [rbp-84], edi
        mov     QWORD PTR [rbp-96], rsi
        lea     rax, [rbp-80]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-80]
        mov     QWORD PTR [rbp-16], rax
.L12:
        mov     rax, QWORD PTR [rbp-8]
        lea     rdx, [rax+1]
        mov     QWORD PTR [rbp-8], rdx
        mov     edx, 1
        mov     rsi, rax
        mov     edi, 0
        mov     eax, 0
        call    read
        mov     DWORD PTR [rbp-28], eax
        cmp     DWORD PTR [rbp-28], -1
        je      .L7
        cmp     DWORD PTR [rbp-28], 0
        jne     .L8
        mov     edi, OFFSET FLAT:.LC0
        call    puts
        mov     edi, 1
        call    exit
.L7:
        mov     edi, OFFSET FLAT:.LC1
        call    puts
        mov     edi, 1
        call    exit
.L8:
        lea     rax, [rbp-80]
        mov     rdx, QWORD PTR [rbp-8]
        sub     rdx, rax
        cmp     rdx, 32
        jne     .L9
        mov     edi, OFFSET FLAT:.LC2
        call    puts
        mov     edi, 1
        call    exit
.L9:
        mov     rax, QWORD PTR [rbp-8]
        sub     rax, 1
        movzx   eax, BYTE PTR [rax]
        cmp     al, 10
        jne     .L12
        mov     rax, QWORD PTR [rbp-8]
        sub     rax, 1
        mov     BYTE PTR [rax], 0
        nop
        jmp     .L13
.L15:
        add     QWORD PTR [rbp-16], 1
.L13:
        mov     rax, QWORD PTR [rbp-16]
        movzx   eax, BYTE PTR [rax]
        cmp     al, 32
        je      .L14
        lea     rax, [rbp-80]
        mov     rdx, QWORD PTR [rbp-16]
        sub     rdx, rax
        mov     rax, rdx
        cmp     rax, 31
        jbe     .L15
.L14:
        lea     rax, [rbp-80]
        mov     rdx, QWORD PTR [rbp-16]
        sub     rdx, rax
        cmp     rdx, 32
        jne     .L16
        mov     edi, OFFSET FLAT:.LC3
        call    puts
        mov     edi, 2
        call    exit
.L16:
        mov     rax, QWORD PTR [rbp-16]
        mov     BYTE PTR [rax], 0
        add     QWORD PTR [rbp-16], 1
        lea     rax, [rbp-80]
        mov     rdx, QWORD PTR [rbp-16]
        sub     rdx, rax
        cmp     rdx, 1
        jle     .L17
        mov     rax, QWORD PTR [rbp-8]
        sub     rax, QWORD PTR [rbp-16]
        cmp     rax, 1
        jg      .L18
.L17:
        mov     edi, OFFSET FLAT:.LC4
        call    puts
        mov     edi, 2
        call    exit
.L18:
        lea     rax, [rbp-80]
        mov     QWORD PTR [rbp-24], rax
        jmp     .L19
.L21:
        mov     rax, QWORD PTR [rbp-24]
        movzx   eax, BYTE PTR [rax]
        movsx   eax, al
        mov     edi, eax
        call    isNumericChar
        test    eax, eax
        je      .L20
        mov     edi, OFFSET FLAT:.LC5
        call    puts
        mov     edi, 2
        call    exit
.L20:
        add     QWORD PTR [rbp-24], 1
.L19:
        mov     rax, QWORD PTR [rbp-24]
        cmp     rax, QWORD PTR [rbp-8]
        jb      .L21
        lea     rax, [rbp-80]
        mov     rdi, rax
        mov     eax, 0
        call    atoi
        mov     DWORD PTR [rbp-32], eax
        mov     rax, QWORD PTR [rbp-16]
        mov     rdi, rax
        mov     eax, 0
        call    atoi
        mov     DWORD PTR [rbp-36], eax
        mov     edx, DWORD PTR [rbp-32]
        mov     eax, DWORD PTR [rbp-36]
        add     eax, edx
        mov     esi, eax
        mov     edi, OFFSET FLAT:.LC6
        mov     eax, 0
        call    printf
        mov     edi, 0
        call    exit
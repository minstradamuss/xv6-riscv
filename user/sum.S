isWhitespace:
        addi    sp,sp,-32
        sd      s0,24(sp)
        addi    s0,sp,32
        mv      a5,a0
        sb      a5,-17(s0)
        lbu     a5,-17(s0)
        andi    a4,a5,0xff
        li      a5,32
        beq     a4,a5,.L2
        lbu     a5,-17(s0)
        andi    a4,a5,0xff
        li      a5,10
        beq     a4,a5,.L2
        lbu     a5,-17(s0)
        andi    a5,a5,0xff
        beq     a5,zero,.L2
        lbu     a5,-17(s0)
        andi    a4,a5,0xff
        li      a5,13
        bne     a4,a5,.L3
.L2:
        li      a5,1
        j       .L4
.L3:
        li      a5,0
.L4:
        mv      a0,a5
        ld      s0,24(sp)
        addi    sp,sp,32
        jr      ra
isDigit:
        addi    sp,sp,-32
        sd      s0,24(sp)
        addi    s0,sp,32
        mv      a5,a0
        sb      a5,-17(s0)
        lbu     a5,-17(s0)
        andi    a4,a5,0xff
        li      a5,47
        bleu    a4,a5,.L7
        lbu     a5,-17(s0)
        andi    a4,a5,0xff
        li      a5,57
        bgtu    a4,a5,.L7
        li      a5,1
        j       .L8
.L7:
        li      a5,0
.L8:
        mv      a0,a5
        ld      s0,24(sp)
        addi    sp,sp,32
        jr      ra
.LC0:
        .string "error: there is only one number given, must be two\n"
.LC1:
        .string "error: invalid syntax\n"
.LC2:
        .string "error: buffer overflow\n"
.LC3:
        .string "%d\n"
main:
        addi    sp,sp,-96
        sd      ra,88(sp)
        sd      s0,80(sp)
        sd      s1,72(sp)
        addi    s0,sp,96
        mv      a5,a0
        sd      a1,-96(s0)
        sw      a5,-84(s0)
        addi    a5,s0,-80
        li      a1,23
        mv      a0,a5
        call    gets
        addi    a5,s0,-80
        addi    a5,a5,22
        sd      a5,-56(s0)
        addi    a5,s0,-80
        sd      a5,-40(s0)
        nop
        ld      a4,-40(s0)
        ld      a5,-56(s0)
        beq     a4,a5,.L15
        ld      a5,-40(s0)
        lbu     a5,0(a5)
        mv      a0,a5
        call    isWhitespace
        mv      a5,a0
        beq     a5,zero,.L12
        li      a2,50
        lui     a5,%hi(.LC0)
        addi    a1,a5,%lo(.LC0)
        li      a0,2
        call    write
        li      a0,1
        call    exit
.L12:
        ld      a5,-40(s0)
        lbu     a5,0(a5)
        mv      a0,a5
        call    isDigit
        mv      a5,a0
        beq     a5,zero,.L13
        ld      a5,-40(s0)
        addi    a5,a5,1
        sd      a5,-40(s0)
.L13:
        ld      a5,-40(s0)
        lbu     a5,0(a5)
        mv      a4,a5
        li      a5,32
        beq     a4,a5,.L23
        li      a2,21
        lui     a5,%hi(.LC1)
        addi    a1,a5,%lo(.LC1)
        li      a0,2
        call    write
        li      a0,3
        call    exit
.L23:
        nop
.L15:
        ld      a5,-40(s0)
        addi    a4,a5,1
        sd      a4,-40(s0)
        sd      a5,-48(s0)
        j       .L17
.L21:
        ld      a5,-48(s0)
        lbu     a5,0(a5)
        mv      a0,a5
        call    isWhitespace
        mv      a5,a0
        bne     a5,zero,.L24
        ld      a5,-48(s0)
        lbu     a5,0(a5)
        mv      a0,a5
        call    isDigit
        mv      a5,a0
        beq     a5,zero,.L20
        ld      a5,-48(s0)
        addi    a5,a5,1
        sd      a5,-48(s0)
        j       .L17
.L20:
        li      a2,22
        lui     a5,%hi(.LC1)
        addi    a1,a5,%lo(.LC1)
        li      a0,2
        call    write
        li      a0,3
        call    exit
.L17:
        ld      a4,-48(s0)
        ld      a5,-56(s0)
        bltu    a4,a5,.L21
        j       .L19
.L24:
        nop
.L19:
        ld      a4,-48(s0)
        ld      a5,-56(s0)
        bltu    a4,a5,.L22
        li      a2,23
        lui     a5,%hi(.LC2)
        addi    a1,a5,%lo(.LC2)
        li      a0,2
        call    write
        li      a0,2
        call    exit
.L22:
        addi    a5,s0,-80
        mv      a0,a5
        call    atoi
        mv      a5,a0
        mv      s1,a5
        ld      a0,-40(s0)
        call    atoi
        mv      a5,a0
        mv      a1,a5
        mv      a0,s1
        call    add
        mv      a5,a0
        mv      a1,a5
        lui     a5,%hi(.LC3)
        addi    a0,a5,%lo(.LC3)
        call    printf
        li      a0,0
        call    exit

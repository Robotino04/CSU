; data(b) -= data(a)
.macro sub a, b{
    subleq a, b
}

; data(b) += data(a)
.macro add a, b{
    sub a, zero
    sub zero, b
    sub zero, zero
}

; pc = a
.macro jmp a{
    subleq zero, zero, a
}


; if (data(x) <= 0) pc = a
.macro jmpnp x, a{
    subleq zero, x, a
}

; if (data(x) > 0) pc = a
.macro jmpp x, address{
    jmpnp x, end
    jmp address
    end:
}

; data(b) = data(a)
.macro mov a, b{
    sub b, b
    add a, b
}

; data(x) = 0
.macro clear x{
    sub x, x
}
.macro halt{
    jmp $0x8000
}

; pc = data(c)
.macro jmpi x{
    mov x, next_instruction + 4
    mov x+1, next_instruction + 5
    next_instruction:
    subleq zero, zero, $0x0000
}

; data(b) *= data(a)
.macro mul a, b{
    mov b, tmp1
    clear tmp2
    loop:
        jmpnp tmp1, end
        add a, tmp2
        sub one, tmp1
        jmp loop

    end:
    mov tmp2, b
}

; data(a + data(b)) = data(c)
.macro movo a, b, c{
    address:
        .data a
    mov address, next_instruction
    add b, next_instruction

    // add
    clear c
    next_instruction:
    subleq a, zero
    sub zero, b
    clear zero
}

mul value_1, value_2
mul value_2, value_3

halt



value_1:
    .data 3
value_2:
    .data 7
value_3:
    .data 3

; temporary variables used by mul
tmp1:
    .data 0
tmp2:
    .data 0

zero:
    .data 0
one:
    .data 1

exit_address:
    .address $0x8000

.org $41 + $69 * 100
.data 0x69
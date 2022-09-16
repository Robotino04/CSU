// data(b) -= data(a)
.macro sub a, b{
    subleq a, b
}

// data(b) += data(a)
.macro add a, b{
    sub a, zero
    sub zero, b
    sub zero, zero
}

// pc = a
.macro jmp a{
    subleq zero, zero, a
}


// if (data(x) <= 0) pc = a
.macro jmpnp x, a{
    subleq zero, x, a
}

// if (data(x) > 0) pc = a
.macro jmpp x, address{
    jmpnp x, end
    jmp address
    end:
}

// data(b) = data(a)
.macro mov a, b{
    sub b, b
    add a, b
}

// data(x) = 0
.macro clear x{
    sub x, x
}
// stops execution
.macro halt{
    jmp $0x8000
}

// pc = data(c)
.macro jmpi x{
    mov x, next_instruction + 4
    mov x+1, next_instruction + 5
    next_instruction:
    subleq zero, zero, $0x0000
}

// data(b) *= data(a)
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

// data(result) += data(address + data(offset))
.macro addo address, offset, result{
    jmp over_address_label
    address_label:
        .address address
    
    over_address_label:
    mov address_label, final_address
    mov address_label+1, final_address+1
    add offset, final_address


    // add
    final_address:
    sub $0x0000, zero
    sub zero, result
    clear zero
}

mov zero, i
mov message_len, tmp1
print_loop:
    mov tmp1, message_len
    addo message, i, $0x8000

    add one, i
    mov message_len, tmp1

    sub i, message_len
    jmpp message_len, print_loop

halt


value_1:
    .data 3
value_2:
    .data 7
value_3:
    .data 3

// temporary variables used by mul
tmp1:
    .data 0
tmp2:
    .data 0

zero:
    .data 0
one:
    .data 1

i:
    .data 0

message:
    .asciiz "Hello, World!\n"
message_len:
    .data message_len - message
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

// data(result + data(result_offset)) += data(address + data(offset))
.macro addo address, offset, result, result_offset{
    jmp over_address_label
    address_label:
        .address address
    result_label:
        .address result
    
    over_address_label:
    // calculate source address
    mov address_label, final_address
    mov address_label+1, final_address + 1
    add offset, final_address

    // calculate result address
    mov result_label, final_result+2
    mov result_label+1, final_result+2 + 1
    add result_offset, final_result+2


    // add
    final_address:
    sub $0x0000, zero
    final_result:
    sub zero, $0x0000
    clear zero
}

mov message_len, i
sub two, i
read_loop:
    add one, i
    addo $0x8001, zero, message, i
    
    clear tmp2
    addo message, i, tmp2, zero
    sub newline, tmp2
    jmpp tmp2, read_loop
    add one, tmp2
    jmpnp i, read_loop


// insert newline
add one, i
addo newline, zero, message, i


mov i, message_len

mov zero, i
mov message_len, tmp1
print_loop:
    mov tmp1, message_len
    addo message, i, $0x8000, zero

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
i:
    .data 0


zero:
    .data 0
one:
    .data 1
two:
    .data 2
newline:
    .data 10



message_len:
    .data message_end - message

message:
    .asciiz "Hello "
message_end:


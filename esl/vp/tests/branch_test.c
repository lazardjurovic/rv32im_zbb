int main(){

    asm volatile(
        "addi x1,x0,8;"
        "addi x2,x0,22;"
        "addi x3,x0,8;"
        "beq x1,x2,8;"
        "addi x1,x0,10;"
        "addi x1,x0,11;"
        "blt x1,x2,8;"
        "addi x1,x0,10;"
        "addi x1,x0,12;"
        "bltu x1,x2,8;"
        "addi x1,x0,10;"
        "addi x1,x0,11;"
        "bgeu x1,x2,8;"
        "addi x1,x0,10;"
        "addi x1,x0,11;"
        "jal x0,8;"
        "addi x1,x0,10;"
        "addi x1,x0,11;"
        "jalr x0,x3,68;"
        "addi x1,x0,10;"
        "addi x1,x0,11;"
        "addi x1,x0,12;"       
    );

    return 0;
}

/*
LB_i_39	0	1
LH_i_40	0	1
SB_i_48	0	1
SH_i_49	0	1

*/
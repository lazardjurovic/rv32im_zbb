int main(){

    asm volatile(
        "addi x1,x0,7;"
        "addi x2,x0,22;"
        "sll x3,x1,x3;"
        "slt x4,x1,x2;"
        "sltu x5,x2,x2;"
        "xor x6,x1,x2;"
        "xori x7,x2,5;"
        "srl x8,x2,x1;"
        "or x9,x1,x2;"
        "and x10,x1,x2;"
        "sub x11,x2,x1;"
        "sra x12,x1,x2;"
        "slli x13,x1,15;"
        "slti x14,x1,2;"
        "sltiu x15,x1,4;"
        "srli x16,x1,11;"
        "ori x17,x1,12;"
        "andi x18,x1,13;"
        "auipc x19, 0x100;"
        
    );

    return 0;
}
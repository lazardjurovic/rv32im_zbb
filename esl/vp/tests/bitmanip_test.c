int main() {

    asm volatile(
        "addi x1,x0,0x123;"
        "addi x2,x0,0x347;"
        "andn x3,x1,x2;"
        "clz x4,x3;"
        "cpop x5,x1;"
        "ctz x6,x2;"
        "max x7,x6,x5;"
        "maxu x8,x6,x5;"
        "min x9,x2,x1;"
        "minu x10,x4,x3;"
        //"orc.b x9,x1,x2;"
        "orn x11,x1,x2;"
        "rev8 x12,x1;"
        "rol x13,x3,x4;"
        "ror x14,x1,x2;"
        "rori x15,x12,5;"
        "sext.b x16,x1;"
        "sext.h x17,x2;"
        "xnor x18,x1,x2;"
        "zext.h x19,x1;"
    );

    return 0;
}

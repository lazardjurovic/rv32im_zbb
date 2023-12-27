int main() {

    asm volatile(
        "addi x1,x0,0x123;"
        "addi x2,x0,0x347;"
        "andn x3,x1,x2;"
        "clz x4,x3;"
        "cpop x5,x1;"
        "ctz x6,x2;"
        "max x7,x6,x5;"
        "maxu x7,x6,x5;"
        "min x8,x2,x1;"
        "minu x9,x4,x3;"
        //"orc.b x9,x1,x2;"
        "orn x10,x1,x2;"
        "rev8 x11,x1;"
        "rol x12,x3,x4;"
        "ror x12,x1,x2;"
        "rori x13,x12,5;"
        "sext.b x14,x1;"
        "sext.h x15,x2;"
        "xnor x16,x1,x2;"
        "zext.h x17,x1;"
    );

    return 0;
}

int main(){

    asm volatile(
        "addi x1,x0,5;"
        "addi x2,x0,10;"
        "lui x8,50;"
        "lui x9,234;"
        "add x5,x1,x8;"
        "add x6,x2,x9;"
        //"add x15,x1,x2;"
        "mul x3, x1,x2;"
        "mulh x4,x1,x2;"
        
        "mulhu x10,x5,x6;"
        "mulhsu x11,x5,x6;"
        
        "lui x1,0;"
        "lui x2,0;"
        /*
        "addi x1,x0,5;"
        "addi x2,x0,10;"
        "div x7,x2,x1;"
        "divu x8,x2,x1;"
        "addi x1,x1,1;"
        "remu x9,x2,x1;"
        "addi x1,x0,-10;"
        "addi x2,x0,6;"
        "rem x11,x2,x1;"
        */
        //"addi x1,x0,0;" // making ra register sutable for jal
        //"ecall"
        
    );

    return 0;
}
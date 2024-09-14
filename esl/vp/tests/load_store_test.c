int main(){

    asm volatile(
        "addi x1,x0,8;"
        "addi x2,x0,350;"
        "sb x2, 4(x0);"
        "lb x3, 4(x0);"
        "sh x1, 8(x0);"
        "lh x4, 8(x0);"
    );

    return 0;
}

#if defined (CAN_COMPILE_ALTIVEC)
static void Altivec_test (void)
{
    asm volatile ("mtspr 256, %0\n" "vand %%v0, %%v0, %%v0\n" : : "r" (-1));
}

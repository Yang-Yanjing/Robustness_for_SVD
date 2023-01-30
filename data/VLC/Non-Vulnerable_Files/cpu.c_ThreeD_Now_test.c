#if defined (CAN_COMPILE_3DNOW)
VLC_MMX static void ThreeD_Now_test (void)
{
    asm volatile ("pfadd %%mm0,%%mm0\n" "femms\n" : : : "mm0");
}

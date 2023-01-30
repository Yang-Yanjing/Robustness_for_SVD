#if defined (CAN_COMPILE_SSE) && !defined (__SSE__)
VLC_SSE static void SSE_test (void)
{
    asm volatile ("xorps %%xmm0,%%xmm0\n" : : : "xmm0", "xmm1");
}

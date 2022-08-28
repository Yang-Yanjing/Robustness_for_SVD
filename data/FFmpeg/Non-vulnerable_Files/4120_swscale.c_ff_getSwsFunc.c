SwsFunc ff_getSwsFunc(SwsContext *c)
{
    sws_init_swscale(c);
    if (ARCH_PPC)
        ff_sws_init_swscale_ppc(c);
    if (ARCH_X86)
        ff_sws_init_swscale_x86(c);
    return swscale;
}

av_cold void sws_rgb2rgb_init(void)
{
    rgb2rgb_init_c();
    if (ARCH_X86)
        rgb2rgb_init_x86();
}

#endif
void vlc_CPU_dump (vlc_object_t *obj)
{
    char buf[200], *p = buf;
#if defined (__i386__) || defined (__x86_64__)
    if (vlc_CPU_MMX()) p += sprintf (p, "MMX ");
    if (vlc_CPU_MMXEXT()) p += sprintf (p, "MMXEXT ");
    if (vlc_CPU_SSE()) p += sprintf (p, "SSE ");
    if (vlc_CPU_SSE2()) p += sprintf (p, "SSE2 ");
    if (vlc_CPU_SSE3()) p += sprintf (p, "SSE3 ");
    if (vlc_CPU_SSSE3()) p += sprintf (p, "SSSE3 ");
    if (vlc_CPU_SSE4_1()) p += sprintf (p, "SSE4.1 ");
    if (vlc_CPU_SSE4_2()) p += sprintf (p, "SSE4.2 ");
    if (vlc_CPU_SSE4A()) p += sprintf (p, "SSE4A ");
    if (vlc_CPU_AVX()) p += sprintf (p, "AVX ");
    if (vlc_CPU_AVX2()) p += sprintf (p, "AVX ");
    if (vlc_CPU_3dNOW()) p += sprintf (p, "3DNow! ");
    if (vlc_CPU_XOP()) p += sprintf (p, "XOP ");
    if (vlc_CPU_FMA4()) p += sprintf (p, "FMA4 ");
#elif defined (__powerpc__) || defined (__ppc__) || defined (__ppc64__)
    if (vlc_CPU_ALTIVEC())  p += sprintf (p, "AltiVec");
#elif defined (__arm__)
    if (vlc_CPU_ARM_NEON()) p += sprintf (p, "ARM_NEON ");
#endif
#if HAVE_FPU
    p += sprintf (p, "FPU ");
#endif
    if (p > buf)
        msg_Dbg (obj, "CPU has capabilities %s", buf);
}

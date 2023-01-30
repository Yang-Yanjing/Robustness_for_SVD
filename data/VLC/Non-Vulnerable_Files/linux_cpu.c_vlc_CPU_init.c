static uint32_t cpu_flags = 0;
static void vlc_CPU_init (void)
{
    FILE *info = fopen ("/proc/cpuinfo", "rte");
    if (info == NULL)
        return;
    char *line = NULL;
    size_t linelen = 0;
    uint_fast32_t all_caps = 0xFFFFFFFF;
    while (getline (&line, &linelen, info) != -1)
    {
        char *p = line, *cap;
        uint_fast32_t core_caps = 0;
#if defined (__arm__)
        unsigned ver;
        if (sscanf (line, "Processor\t: ARMv%u", &ver) >= 1 && ver >= 6)
            core_caps |= VLC_CPU_ARMv6;
#endif
        if (strncmp (line, CPU_FLAGS, strlen (CPU_FLAGS)))
            continue;
        while ((cap = strsep (&p, " ")) != NULL)
        {
#if defined (__arm__)
            if (!strcmp (cap, "neon"))
                core_caps |= VLC_CPU_ARM_NEON;
#elif defined (__i386__) || defined (__x86_64__)
            if (!strcmp (cap, "mmx"))
                core_caps |= VLC_CPU_MMX;
            if (!strcmp (cap, "sse"))
                core_caps |= VLC_CPU_SSE | VLC_CPU_MMXEXT;
            if (!strcmp (cap, "mmxext"))
                core_caps |= VLC_CPU_MMXEXT;
            if (!strcmp (cap, "sse2"))
                core_caps |= VLC_CPU_SSE2;
            if (!strcmp (cap, "pni"))
                core_caps |= VLC_CPU_SSE3;
            if (!strcmp (cap, "ssse3"))
                core_caps |= VLC_CPU_SSSE3;
            if (!strcmp (cap, "sse4_1"))
                core_caps |= VLC_CPU_SSE4_1;
            if (!strcmp (cap, "sse4_2"))
                core_caps |= VLC_CPU_SSE4_2;
            if (!strcmp (cap, "sse4a"))
                core_caps |= VLC_CPU_SSE4A;
            if (!strcmp (cap, "avx"))
                core_caps |= VLC_CPU_AVX;
            if (!strcmp (cap, "avx2"))
                core_caps |= VLC_CPU_AVX2;
            if (!strcmp (cap, "3dnow"))
                core_caps |= VLC_CPU_3dNOW;
            if (!strcmp (cap, "xop"))
                core_caps |= VLC_CPU_XOP;
            if (!strcmp (cap, "fma4"))
                core_caps |= VLC_CPU_FMA4;
#elif defined (__powerpc__) || defined (__powerpc64__)
            if (!strcmp (cap, "altivec supported"))
                core_caps |= VLC_CPU_ALTIVEC;
#endif
        }
        /* Take the intersection of capabilities of each processor */
        all_caps &= core_caps;
    }
    fclose (info);
    free (line);
    if (all_caps == 0xFFFFFFFF) /* Error parsing of cpuinfo? */
        all_caps = 0; /* Do not assume any capability! */
    cpu_flags = all_caps;
}
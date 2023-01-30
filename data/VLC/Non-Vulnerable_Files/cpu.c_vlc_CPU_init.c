 */
void vlc_CPU_init (void)
{
    uint32_t i_capabilities = 0;
#if defined( __i386__ ) || defined( __x86_64__ )
     unsigned int i_eax, i_ebx, i_ecx, i_edx;
     bool b_amd;
    /* Needed for x86 CPU capabilities detection */
# if defined (__i386__) && defined (__PIC__)
#  define cpuid(reg) \
     asm volatile ("xchgl %%ebx,%1\n\t" \
                   "cpuid\n\t" \
                   "xchgl %%ebx,%1\n\t" \
                   : "=a" (i_eax), "=r" (i_ebx), "=c" (i_ecx), "=d" (i_edx) \
                   : "a" (reg) \
                   : "cc");
# else
#  define cpuid(reg) \
     asm volatile ("cpuid\n\t" \
                   : "=a" (i_eax), "=b" (i_ebx), "=c" (i_ecx), "=d" (i_edx) \
                   : "a" (reg) \
                   : "cc");
# endif
     /* Check if the OS really supports the requested instructions */
# if defined (__i386__) && !defined (__i486__) && !defined (__i586__) \
  && !defined (__i686__) && !defined (__pentium4__) \
  && !defined (__k6__) && !defined (__athlon__) && !defined (__k8__)
    /* check if cpuid instruction is supported */
    asm volatile ( "push %%ebx\n\t"
                   "pushf\n\t"
                   "pop %%eax\n\t"
                   "movl %%eax, %%ebx\n\t"
                   "xorl $0x200000, %%eax\n\t"
                   "push %%eax\n\t"
                   "popf\n\t"
                   "pushf\n\t"
                   "pop %%eax\n\t"
                   "movl %%ebx,%1\n\t"
                   "pop %%ebx\n\t"
                 : "=a" ( i_eax ),
                   "=r" ( i_ebx )
                 :
                 : "cc" );
    if( i_eax == i_ebx )
        goto out;
# endif
    /* the CPU supports the CPUID instruction - get its level */
    cpuid( 0x00000000 );
# if defined (__i386__) && !defined (__i586__) \
  && !defined (__i686__) && !defined (__pentium4__) \
  && !defined (__k6__) && !defined (__athlon__) && !defined (__k8__)
    if( !i_eax )
        goto out;
#endif
    /* borrowed from mpeg2dec */
    b_amd = ( i_ebx == 0x68747541 ) && ( i_ecx == 0x444d4163 )
                    && ( i_edx == 0x69746e65 );
    /* test for the MMX flag */
    cpuid( 0x00000001 );
# if !defined (__MMX__)
    if( ! (i_edx & 0x00800000) )
        goto out;
# endif
    i_capabilities |= VLC_CPU_MMX;
    if( i_edx & 0x02000000 )
        i_capabilities |= VLC_CPU_MMXEXT;
# if defined (CAN_COMPILE_SSE) && !defined (__SSE__)
    if (( i_edx & 0x02000000 ) && vlc_CPU_check ("SSE", SSE_test))
# endif
    {
        /*if( i_edx & 0x02000000 )*/
            i_capabilities |= VLC_CPU_SSE;
        if (i_edx & 0x04000000)
            i_capabilities |= VLC_CPU_SSE2;
        if (i_ecx & 0x00000001)
            i_capabilities |= VLC_CPU_SSE3;
        if (i_ecx & 0x00000200)
            i_capabilities |= VLC_CPU_SSSE3;
        if (i_ecx & 0x00080000)
            i_capabilities |= VLC_CPU_SSE4_1;
        if (i_ecx & 0x00100000)
            i_capabilities |= VLC_CPU_SSE4_2;
    }
    /* test for additional capabilities */
    cpuid( 0x80000000 );
    if( i_eax < 0x80000001 )
        goto out;
    /* list these additional capabilities */
    cpuid( 0x80000001 );
# if defined (CAN_COMPILE_3DNOW) && !defined (__3dNOW__)
    if ((i_edx & 0x80000000) && vlc_CPU_check ("3D Now!", ThreeD_Now_test))
# endif
        i_capabilities |= VLC_CPU_3dNOW;
    if( b_amd && ( i_edx & 0x00400000 ) )
        i_capabilities |= VLC_CPU_MMXEXT;
out:
#elif defined( __powerpc__ ) || defined( __ppc__ ) || defined( __powerpc64__ ) \
    || defined( __ppc64__ )
#   if defined(__APPLE__) || defined(__OpenBSD__)
#   if defined(__OpenBSD__)
    int selectors[2] = { CTL_MACHDEP, CPU_ALTIVEC };
#   else
    int selectors[2] = { CTL_HW, HW_VECTORUNIT };
#   endif
    int i_has_altivec = 0;
    size_t i_length = sizeof( i_has_altivec );
    int i_error = sysctl( selectors, 2, &i_has_altivec, &i_length, NULL, 0);
    if( i_error == 0 && i_has_altivec != 0 )
        i_capabilities |= VLC_CPU_ALTIVEC;
#   elif defined( CAN_COMPILE_ALTIVEC )
    if (vlc_CPU_check ("Altivec", Altivec_test))
        i_capabilities |= VLC_CPU_ALTIVEC;
#   endif
#elif defined ( __arm__)
# ifdef __ANDROID__
    if (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON)
        i_capabilities |= VLC_CPU_ARM_NEON;
# endif
#endif
    cpu_flags = i_capabilities;
}

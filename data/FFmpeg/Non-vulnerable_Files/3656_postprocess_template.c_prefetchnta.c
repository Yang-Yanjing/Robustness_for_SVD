static inline void RENAME(prefetchnta)(const void *p)
{
    __asm__ volatile(   "prefetchnta (%0)\n\t"
        : : "r" (p)
    );
}
#elif !ARCH_X86 && AV_GCC_VERSION_AT_LEAST(3,2)
static inline void RENAME(prefetchnta)(const void *p)
{
    __builtin_prefetch(p,0,0);
}
#else
static inline void RENAME(prefetchnta)(const void *p)
{
    return;
}

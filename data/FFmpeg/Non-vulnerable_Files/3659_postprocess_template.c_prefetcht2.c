static inline void RENAME(prefetcht2)(const void *p)
{
    __asm__ volatile(   "prefetcht2 (%0)\n\t"
        : : "r" (p)
    );
}
}
static inline void RENAME(prefetcht2)(const void *p)
{
    __builtin_prefetch(p,0,3);
}
}
static inline void RENAME(prefetcht2)(const void *p)
{
    return;
}

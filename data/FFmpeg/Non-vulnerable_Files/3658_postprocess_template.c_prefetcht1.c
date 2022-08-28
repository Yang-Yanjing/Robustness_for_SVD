static inline void RENAME(prefetcht1)(const void *p)
{
    __asm__ volatile(   "prefetcht1 (%0)\n\t"
        : : "r" (p)
    );
}
}
static inline void RENAME(prefetcht1)(const void *p)
{
    __builtin_prefetch(p,0,2);
}
}
static inline void RENAME(prefetcht1)(const void *p)
{
    return;
}

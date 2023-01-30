};
static int keycmp (const void *a, const void *b)
{
    const struct mapping *ka = a, *kb = b;
#if (INT_MAX >= 0x7fffffff)
    return ka->key - kb->key;
#else
    return (ka->key < kb->key) ? -1 : (ka->key > kb->key) ? +1 : 0;
#endif
}

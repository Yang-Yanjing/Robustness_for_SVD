#define BLOCK_WASTE_SIZE   2048
block_t *block_Alloc (size_t size)
{
    /* 2 * BLOCK_PADDING: pre + post padding */
    const size_t alloc = sizeof (block_t) + BLOCK_ALIGN + (2 * BLOCK_PADDING)
                       + size;
    if (unlikely(alloc <= size))
        return NULL;
    block_t *b = malloc (alloc);
    if (unlikely(b == NULL))
        return NULL;
    block_Init (b, b + 1, alloc - sizeof (*b));
    static_assert ((BLOCK_PADDING % BLOCK_ALIGN) == 0,
                   "BLOCK_PADDING must be a multiple of BLOCK_ALIGN");
    b->p_buffer += BLOCK_PADDING + BLOCK_ALIGN - 1;
    b->p_buffer = (void *)(((uintptr_t)b->p_buffer) & ~(BLOCK_ALIGN - 1));
    b->i_buffer = size;
    b->pf_release = block_generic_Release;
    return b;
}

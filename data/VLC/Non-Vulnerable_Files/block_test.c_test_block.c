}
static void test_block (void)
{
    block_t *block = block_Alloc (sizeof (text));
    assert (block != NULL);
    memcpy (block->p_buffer, text, sizeof (text));
    block = block_Realloc (block, 0, sizeof (text));
    assert (block != NULL);
    assert (block->i_buffer == sizeof (text));
    assert (!memcmp (block->p_buffer, text, sizeof (text)));
    block = block_Realloc (block, 200, sizeof (text) + 200);
    assert (block != NULL);
    assert (block->i_buffer == 200 + sizeof (text) + 200);
    assert (!memcmp (block->p_buffer + 200, text, sizeof (text)));
    block = block_Realloc (block, -200, sizeof (text) + 200);
    assert (block != NULL);
    assert (block->i_buffer == sizeof (text));
    assert (!memcmp (block->p_buffer, text, sizeof (text)));
    block_Release (block);
    //block = block_Alloc (SIZE_MAX);
    //assert (block == NULL);
}

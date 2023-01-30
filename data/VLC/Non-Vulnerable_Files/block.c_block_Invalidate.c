}
static void block_Invalidate (block_t *block)
{
    block->p_next = NULL;
    block_Check (block);
    block->pf_release = BlockNoRelease;
}

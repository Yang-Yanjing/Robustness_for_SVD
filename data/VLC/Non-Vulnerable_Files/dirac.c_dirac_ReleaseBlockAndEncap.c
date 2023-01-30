}
static void dirac_ReleaseBlockAndEncap( block_t *p_block )
{
    fake_block_t *p_fake = (fake_block_t *)p_block;
    free( dirac_RemoveBlockEncap( p_block ) );
    block_Release( p_fake->p_orig );
    free( p_fake );
}

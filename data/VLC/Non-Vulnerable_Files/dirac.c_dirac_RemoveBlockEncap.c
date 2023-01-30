} fake_block_t;
static dirac_block_encap_t *dirac_RemoveBlockEncap( block_t *p_block )
{
    fake_block_t *p_fake = (fake_block_t *)p_block;
    dirac_block_encap_t *p_dbe = p_fake->p_dbe;
    p_fake->p_dbe = NULL;
    return p_dbe;
}

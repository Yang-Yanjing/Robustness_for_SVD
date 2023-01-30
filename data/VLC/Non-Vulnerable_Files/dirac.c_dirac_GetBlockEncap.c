}
static dirac_block_encap_t *dirac_GetBlockEncap( block_t *p_block )
{
    return ((fake_block_t *)p_block)->p_dbe;
}

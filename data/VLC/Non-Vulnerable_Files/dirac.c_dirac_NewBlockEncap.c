}
static dirac_block_encap_t *dirac_NewBlockEncap( block_t **pp_block )
{
    dirac_block_encap_t *p_dbe = calloc( 1, sizeof( *p_dbe ) );
    if( p_dbe ) dirac_AddBlockEncap( pp_block, p_dbe );
    return p_dbe;
}

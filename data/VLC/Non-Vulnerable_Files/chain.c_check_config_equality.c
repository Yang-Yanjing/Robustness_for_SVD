};
static void check_config_equality( config_chain_t *p_cfg1, config_chain_t *p_cfg2 )
{
    while(p_cfg1 && p_cfg2)
    {
        assert( !strcmp( p_cfg1->psz_name,  p_cfg2->psz_name ) &&
                !strcmp( p_cfg1->psz_value, p_cfg2->psz_value ) );
        p_cfg1 = p_cfg1->p_next;
        p_cfg2 = p_cfg2->p_next;
    }
    assert(!p_cfg1 && !p_cfg2);
}

}
static void test_config_ChainDuplicate()
{
    for( int i = 0; ppsz_string[i]; i++ )
    {
        char *psz_module;
        config_chain_t *p_cfg;
        char *psz_next = config_ChainCreate( &psz_module, &p_cfg, ppsz_string[i] );
        config_chain_t *p_cfg_copy = config_ChainDuplicate( p_cfg );
        check_config_equality(p_cfg, p_cfg_copy);
        config_ChainDestroy( p_cfg_copy );
        config_ChainDestroy( p_cfg );
        free( psz_next );
        free( psz_module );
    }
}

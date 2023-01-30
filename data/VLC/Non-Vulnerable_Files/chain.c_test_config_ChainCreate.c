};
static void test_config_ChainCreate()
{
    for( int i = 0; chain_samples[i].psz_string; i++ )
    {
        config_chain_t *p_cfg;
        char *psz_module;
        char *psz_next = config_ChainCreate( &psz_module, &p_cfg, chain_samples[i].psz_string );
        assert( !strcmp( chain_samples[i].psz_module, psz_module ) );
        assert( (!psz_next && !chain_samples[i].psz_next) || !strcmp( chain_samples[i].psz_next, psz_next ) );
        config_chain_t *p_tmp = p_cfg;
        for( int j = 0; chain_samples[i].config[j].psz_name; j++)
        {
            assert( !strcmp( chain_samples[i].config[j].psz_name,  p_tmp->psz_name ) &&
                    !strcmp( chain_samples[i].config[j].psz_value, p_tmp->psz_value ) );
            p_tmp = p_tmp->p_next;
        }
        assert( !p_tmp );
        config_ChainDestroy( p_cfg );
        free( psz_next );
        free( psz_module );
    }
}

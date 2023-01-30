 */
module_config_t *module_config_get( const module_t *module, unsigned *restrict psize )
{
    unsigned i,j;
    unsigned size = module->confsize;
    module_config_t *config = malloc( size * sizeof( *config ) );
    assert( psize != NULL );
    *psize = 0;
    if( !config )
        return NULL;
    for( i = 0, j = 0; i < size; i++ )
    {
        const module_config_t *item = module->p_config + i;
        if( item->b_internal /* internal option */
         || item->b_removed /* removed option */ )
            continue;
        memcpy( config + j, item, sizeof( *config ) );
        j++;
    }
    *psize = j;
    return config;
}

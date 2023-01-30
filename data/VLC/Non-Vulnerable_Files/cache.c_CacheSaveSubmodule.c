}
static int CacheSaveSubmodule( FILE *file, const module_t *p_module )
{
    if( !p_module )
        return 0;
    if( CacheSaveSubmodule( file, p_module->next ) )
        goto error;
    SAVE_STRING( p_module->psz_shortname );
    SAVE_STRING( p_module->psz_longname );
    SAVE_IMMEDIATE( p_module->i_shortcuts );
    for( unsigned j = 0; j < p_module->i_shortcuts; j++ )
         SAVE_STRING( p_module->pp_shortcuts[j] );
    SAVE_STRING( p_module->psz_capability );
    SAVE_IMMEDIATE( p_module->i_score );
    return 0;
error:
    return -1;
}

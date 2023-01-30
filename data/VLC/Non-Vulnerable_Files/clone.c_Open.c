 */
static int Open( vlc_object_t *p_this )
{
    video_splitter_t *p_splitter = (video_splitter_t*)p_this;
    config_ChainParse( p_splitter, CFG_PREFIX, ppsz_filter_options,
                       p_splitter->p_cfg );
    char *psz_clonelist = var_CreateGetNonEmptyString( p_splitter,
                                                       CFG_PREFIX "vout-list" );
    if( psz_clonelist )
    {
        /* Count the number of defined vout */
        p_splitter->i_output = 1;
        for( int i = 0; psz_clonelist[i]; i++ )
        {
            if( psz_clonelist[i] == VOUTSEPARATOR )
                p_splitter->i_output++;
        }
        /* */
        p_splitter->p_output = calloc( p_splitter->i_output,
                                       sizeof(*p_splitter->p_output) );
        if( !p_splitter->p_output )
        {
            free( psz_clonelist );
            return VLC_EGENERIC;
        }
        /* Tokenize the list */
        char *psz_tmp = psz_clonelist;
        for( int i = 0; psz_tmp && *psz_tmp; i++ )
        {
            char *psz_new = strchr( psz_tmp, VOUTSEPARATOR );
            if( psz_new )
                *psz_new++ = '\0';
            p_splitter->p_output[i].psz_module = strdup( psz_tmp );
            psz_tmp = psz_new;
        }
        free( psz_clonelist );
    }
    else
    {
        /* No list was specified. We will use the default vout, and get
         * the number of clones from clone-count */
        p_splitter->i_output = var_CreateGetInteger( p_splitter, CFG_PREFIX "count" );
        if( p_splitter->i_output <= 0 )
            p_splitter->i_output = 1;
        p_splitter->p_output = calloc( p_splitter->i_output,
                                       sizeof(*p_splitter->p_output) );
        if( !p_splitter->p_output )
            return VLC_EGENERIC;
        for( int i = 0; i < p_splitter->i_output; i++ )
            p_splitter->p_output[i].psz_module = NULL;
    }
    /* */
    for( int i = 0; i < p_splitter->i_output; i++ )
    {
        video_splitter_output_t *p_cfg = &p_splitter->p_output[i];
        video_format_Copy( &p_cfg->fmt, &p_splitter->fmt );
        p_cfg->window.i_x = 0;
        p_cfg->window.i_y = 0;
        p_cfg->window.i_align = 0;
    }
    /* */
    p_splitter->pf_filter = Filter;
    p_splitter->pf_mouse  = NULL;
    msg_Dbg( p_splitter, "spawning %i clone(s)", p_splitter->i_output );
    return VLC_SUCCESS;
}

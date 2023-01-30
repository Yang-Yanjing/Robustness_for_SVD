/* Helpers */
static filter_t *filter_chain_AppendFilterInternal( filter_chain_t *p_chain,
                                                    const char *psz_name,
                                                    config_chain_t *p_cfg,
                                                    const es_format_t *p_fmt_in,
                                                    const es_format_t *p_fmt_out )
{
    chained_filter_t *p_chained =
        vlc_custom_create( p_chain->p_this, sizeof(*p_chained), "filter" );
    filter_t *p_filter = &p_chained->filter;
    if( !p_filter )
        return NULL;
    if( !p_fmt_in )
    {
        if( p_chain->last != NULL )
            p_fmt_in = &p_chain->last->filter.fmt_out;
        else
            p_fmt_in = &p_chain->fmt_in;
    }
    if( !p_fmt_out )
    {
        p_fmt_out = &p_chain->fmt_out;
    }
    es_format_Copy( &p_filter->fmt_in, p_fmt_in );
    es_format_Copy( &p_filter->fmt_out, p_fmt_out );
    p_filter->p_cfg = p_cfg;
    p_filter->b_allow_fmt_out_change = p_chain->b_allow_fmt_out_change;
    p_filter->p_module = module_need( p_filter, p_chain->psz_capability,
                                      psz_name, psz_name != NULL );
    if( !p_filter->p_module )
        goto error;
    if( p_filter->b_allow_fmt_out_change )
    {
        es_format_Clean( &p_chain->fmt_out );
        es_format_Copy( &p_chain->fmt_out, &p_filter->fmt_out );
    }
    if( AllocatorInit( &p_chain->allocator, p_chained ) )
        goto error;
    if( p_chain->last == NULL )
    {
        assert( p_chain->first == NULL );
        p_chain->first = p_chained;
    }
    else
        p_chain->last->next = p_chained;
    p_chained->prev = p_chain->last;
    p_chain->last = p_chained;
    p_chained->next = NULL;
    p_chain->length++;
    vlc_mouse_t *p_mouse = malloc( sizeof(*p_mouse) );
    if( p_mouse )
        vlc_mouse_Init( p_mouse );
    p_chained->mouse = p_mouse;
    p_chained->pending = NULL;
    msg_Dbg( p_chain->p_this, "Filter '%s' (%p) appended to chain",
             psz_name ? psz_name : module_get_name(p_filter->p_module, false),
             p_filter );
    return p_filter;
error:
    if( psz_name )
        msg_Err( p_chain->p_this, "Failed to create %s '%s'",
                 p_chain->psz_capability, psz_name );
    else
        msg_Err( p_chain->p_this, "Failed to create %s",
                 p_chain->psz_capability );
    if( p_filter->p_module )
        module_unneed( p_filter, p_filter->p_module );
    es_format_Clean( &p_filter->fmt_in );
    es_format_Clean( &p_filter->fmt_out );
    vlc_object_release( p_filter );
    return NULL;
}

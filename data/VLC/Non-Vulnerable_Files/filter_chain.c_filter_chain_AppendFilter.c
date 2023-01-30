}
filter_t *filter_chain_AppendFilter( filter_chain_t *p_chain,
                                     const char *psz_name,
                                     config_chain_t *p_cfg,
                                     const es_format_t *p_fmt_in,
                                     const es_format_t *p_fmt_out )
{
    filter_t *p_filter = filter_chain_AppendFilterInternal( p_chain, psz_name,
                                                            p_cfg, p_fmt_in,
                                                            p_fmt_out );
    if( UpdateBufferFunctions( p_chain ) < 0 )
        msg_Err( p_filter, "Woah! This doesn't look good." );
    return p_filter;
}

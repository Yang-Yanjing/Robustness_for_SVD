}
const es_format_t *filter_chain_GetFmtOut( filter_chain_t *p_chain )
{
    if( p_chain->b_allow_fmt_out_change )
        return &p_chain->fmt_out;
    if( p_chain->last != NULL )
        return &p_chain->last->filter.fmt_out;
    /* Unless filter_chain_Reset has been called we are doomed */
    return &p_chain->fmt_out;
}

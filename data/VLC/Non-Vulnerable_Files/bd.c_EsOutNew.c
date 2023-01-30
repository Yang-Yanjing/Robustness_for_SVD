}
static es_out_t *EsOutNew( demux_t *p_demux )
{
    es_out_t *p_out = malloc( sizeof(*p_out) );
    es_out_sys_t *p_sys;
    if( !p_out )
        return NULL;
    p_out->pf_add     = EsOutAdd;
    p_out->pf_send    = EsOutSend;
    p_out->pf_del     = EsOutDel;
    p_out->pf_control = EsOutControl;
    p_out->pf_destroy = EsOutDestroy;
    p_out->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
    {
        free( p_out );
        return NULL;
    }
    p_sys->p_demux = p_demux;
    return p_out;
}

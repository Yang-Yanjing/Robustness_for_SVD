}
static es_out_t *esOutNew(demux_t *p_demux)
{
    assert(vlc_array_count(&p_demux->p_sys->es) == 0);
    es_out_t    *p_out = malloc(sizeof(*p_out));
    if (unlikely(p_out == NULL))
        return NULL;
    p_out->pf_add       = esOutAdd;
    p_out->pf_control   = esOutControl;
    p_out->pf_del       = esOutDel;
    p_out->pf_destroy   = esOutDestroy;
    p_out->pf_send      = esOutSend;
    p_out->p_sys = malloc(sizeof(*p_out->p_sys));
    if (unlikely(p_out->p_sys == NULL)) {
        free(p_out);
        return NULL;
    }
    p_out->p_sys->p_demux = p_demux;
    return p_out;
}

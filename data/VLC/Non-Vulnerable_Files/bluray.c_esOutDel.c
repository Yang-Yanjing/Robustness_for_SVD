}
static void esOutDel(es_out_t *p_out, es_out_id_t *p_es)
{
    int idx = findEsPairIndexByEs(p_out->p_sys->p_demux->p_sys, p_es);
    if (idx >= 0) {
        free(vlc_array_item_at_index(&p_out->p_sys->p_demux->p_sys->es, idx));
        vlc_array_remove(&p_out->p_sys->p_demux->p_sys->es, idx);
    }
    es_out_Del(p_out->p_sys->p_demux->out, p_es);
}

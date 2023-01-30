}
static void esOutDestroy(es_out_t *p_out)
{
    for (int i = 0; i < vlc_array_count(&p_out->p_sys->p_demux->p_sys->es); ++i)
        free(vlc_array_item_at_index(&p_out->p_sys->p_demux->p_sys->es, i));
    vlc_array_clear(&p_out->p_sys->p_demux->p_sys->es);
    free(p_out->p_sys);
    free(p_out);
}

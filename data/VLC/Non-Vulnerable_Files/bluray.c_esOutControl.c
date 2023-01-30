}
static int esOutControl(es_out_t *p_out, int i_query, va_list args)
{
    return es_out_vaControl(p_out->p_sys->p_demux->out, i_query, args);
}

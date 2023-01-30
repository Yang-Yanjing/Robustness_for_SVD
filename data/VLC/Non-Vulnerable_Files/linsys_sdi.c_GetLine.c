}
static const uint8_t *GetLine( demux_t *p_demux, const uint8_t **pp_parser,
                               const uint8_t *p_end )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    unsigned int i_total_size = p_sys->i_anc_size + p_sys->i_active_size;
    const uint8_t *p_tmp;
    if ( p_sys->i_line_buffer )
    {
        unsigned int i_remaining = i_total_size - p_sys->i_line_buffer;
        memcpy( p_sys->p_line_buffer + p_sys->i_line_buffer,
                                   *pp_parser, i_remaining );
        *pp_parser += i_remaining;
        p_sys->i_line_buffer = 0;
        return p_sys->p_line_buffer;
    }
    if ( p_end - *pp_parser < (int)i_total_size )
    {
        memcpy( p_sys->p_line_buffer, *pp_parser,
                                   p_end - *pp_parser );
        p_sys->i_line_buffer = p_end - *pp_parser;
        return NULL;
    }
    p_tmp = *pp_parser;
    *pp_parser += i_total_size;
    return p_tmp;
}

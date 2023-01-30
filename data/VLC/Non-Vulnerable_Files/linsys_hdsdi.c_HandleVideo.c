}
static int HandleVideo( demux_t *p_demux, const uint8_t *p_buffer )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t *p_current_picture = block_Alloc( p_sys->i_vblock_size );
    if( unlikely( !p_current_picture ) )
        return VLC_ENOMEM;
    uint8_t *p_y = p_current_picture->p_buffer;
    uint8_t *p_u = p_y + p_sys->i_width * p_sys->i_height;
    uint8_t *p_v = p_u + p_sys->i_width * p_sys->i_height / 4;
    unsigned int i_total_size = p_sys->i_width * 2;
    unsigned int i_current_line;
    struct block_extension_t ext;
    for ( i_current_line = 0; i_current_line < p_sys->i_height;
          i_current_line++ )
    {
        bool b_field = (i_current_line >= p_sys->i_height / 2);
        unsigned int i_field_line = b_field ?
            i_current_line - (p_sys->i_height + 1) / 2 :
            i_current_line;
        unsigned int i_real_line = b_field + i_field_line * 2;
        const uint8_t *p_line = p_buffer + i_current_line * p_sys->i_width * 2;
        if ( !(i_field_line % 2) && !b_field )
            Unpack01( p_line, i_total_size,
                      p_y + p_sys->i_width * i_real_line,
                      p_u + (p_sys->i_width / 2) * (i_real_line / 2),
                      p_v + (p_sys->i_width / 2) * (i_real_line / 2) );
        else if ( !(i_field_line % 2) )
            Unpack01( p_line, i_total_size,
                      p_y + p_sys->i_width * i_real_line,
                      p_u + (p_sys->i_width / 2) * (i_real_line / 2 + 1),
                      p_v + (p_sys->i_width / 2) * (i_real_line / 2 + 1) );
       else if ( !b_field )
            Unpack2( p_line, i_total_size,
                     p_y + p_sys->i_width * i_real_line,
                     p_u + (p_sys->i_width / 2) * (i_real_line / 2 - 1),
                     p_v + (p_sys->i_width / 2) * (i_real_line / 2 - 1) );
       else
            Unpack3( p_line, i_total_size,
                     p_y + p_sys->i_width * i_real_line,
                     p_u + (p_sys->i_width / 2) * (i_real_line / 2),
                     p_v + (p_sys->i_width / 2) * (i_real_line / 2) );
    }
    /* FIXME: progressive formats ? */
    ext.b_progressive = false;
    ext.i_nb_fields = 2;
    ext.b_top_field_first = true;
    ext.i_aspect = p_sys->i_forced_aspect ? p_sys->i_forced_aspect :
                   p_sys->i_aspect;
    memcpy( &p_current_picture->p_buffer[p_sys->i_vblock_size
                                          - sizeof(struct block_extension_t)],
            &ext, sizeof(struct block_extension_t) );
    p_current_picture->i_dts = p_current_picture->i_pts = p_sys->i_next_vdate;
    es_out_Send( p_demux->out, p_sys->p_es_video, p_current_picture );
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_sys->i_next_vdate );
    p_sys->i_next_vdate += p_sys->i_incr;
    return VLC_SUCCESS;
}

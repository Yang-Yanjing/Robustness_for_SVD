}
static void DecodeVideo( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    struct block_extension_t ext;
    /* FIXME: progressive formats ? */
    ext.b_progressive     = false;
    ext.i_nb_fields       = 2;
    ext.b_top_field_first = true;
    ext.i_aspect = p_sys->i_forced_aspect ? p_sys->i_forced_aspect :
                   p_sys->i_aspect;
    memcpy( &p_sys->p_current_picture->p_buffer[p_sys->i_block_size
                                     - sizeof(struct block_extension_t)],
            &ext, sizeof(struct block_extension_t) );
    p_sys->p_current_picture->i_dts = p_sys->p_current_picture->i_pts
        = p_sys->i_next_date;
    es_out_Send( p_demux->out, p_sys->p_es_video, p_sys->p_current_picture );
}

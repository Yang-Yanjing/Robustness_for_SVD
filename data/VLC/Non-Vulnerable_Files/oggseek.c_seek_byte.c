/* seek in ogg file to offset i_pos and update the sync */
static void seek_byte( demux_t *p_demux, int64_t i_pos )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    if ( ! stream_Seek( p_demux->s, i_pos ) )
    {
        ogg_sync_reset( &p_sys->oy );
        p_sys->i_input_position = i_pos;
        p_sys->b_page_waiting = false;
    }
}

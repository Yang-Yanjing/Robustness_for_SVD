}
static inline bool NeedsPacketTable( demux_sys_t *p_sys )
{
    return ( !p_sys->fmt.audio.i_bytes_per_frame || !p_sys->fmt.audio.i_frame_length );
}

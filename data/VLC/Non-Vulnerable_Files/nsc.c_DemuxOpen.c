}
static int DemuxOpen( vlc_object_t * p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    const uint8_t *p_peek;
    int i_size;
    /* Lets check the content to see if this is a NSC file */
    i_size = stream_Peek( p_demux->s, &p_peek, MAX_LINE );
    i_size -= sizeof("NSC Format Version=") - 1;
    if ( i_size > 0 )
    {
        while ( i_size && strncasecmp( (char *)p_peek, "NSC Format Version=",
                                       (int) sizeof("NSC Format Version=") - 1 ) )
        {
            p_peek++;
            i_size--;
        }
        if ( !strncasecmp( (char *)p_peek, "NSC Format Version=",
                           (int) sizeof("NSC Format Version=") -1 ) )
        {
            p_demux->pf_demux = Demux;
            p_demux->pf_control = Control;
            return VLC_SUCCESS;
        }
    }
    return VLC_EGENERIC;
}

 *****************************************************************************/
int Import_GVP( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    int i_peek, i, b_found = false;
    const uint8_t *p_peek;
    i_peek = stream_Peek( p_demux->s, &p_peek, MAX_LINE );
    for( i = 0; i < i_peek - (int)sizeof("gvp_version:"); i++ )
    {
        if( p_peek[i] == 'g' && p_peek[i+1] == 'v' && p_peek[i+2] == 'p' &&
            !memcmp( p_peek+i, "gvp_version:", sizeof("gvp_version:") - 1 ) )
        {
            b_found = true;
            break;
        }
    }
    if( !b_found ) return VLC_EGENERIC;
    STANDARD_DEMUX_INIT_MSG(  "using Google Video Playlist (gvp) import" );
    p_demux->pf_control = Control;
    p_demux->pf_demux = Demux;
    p_demux->p_sys = malloc( sizeof( demux_sys_t ) );
    if( !p_demux->p_sys )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}

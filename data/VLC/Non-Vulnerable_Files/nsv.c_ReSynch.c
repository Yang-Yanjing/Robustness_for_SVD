 *****************************************************************************/
static int ReSynch( demux_t *p_demux )
{
    const uint8_t *p_peek;
    int      i_skip;
    int      i_peek;
    while( vlc_object_alive (p_demux) )
    {
        if( ( i_peek = stream_Peek( p_demux->s, &p_peek, 1024 ) ) < 8 )
        {
            return VLC_EGENERIC;
        }
        i_skip = 0;
        while( i_skip < i_peek - 4 )
        {
            if( !memcmp( p_peek, "NSVf", 4 )
             || !memcmp( p_peek, "NSVs", 4 ) )
            {
                if( i_skip > 0 )
                {
                    stream_Read( p_demux->s, NULL, i_skip );
                }
                return VLC_SUCCESS;
            }
            p_peek++;
            i_skip++;
        }
        stream_Read( p_demux->s, NULL, i_skip );
    }
    return VLC_EGENERIC;
}

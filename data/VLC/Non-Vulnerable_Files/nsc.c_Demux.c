 *****************************************************************************/
static int Demux ( demux_t *p_demux )
{
    char            *psz_line;
    while( ( psz_line = stream_ReadLine( p_demux->s ) ) )
    {
        ParseLine( p_demux, psz_line );
        free( psz_line );
    }
    return VLC_SUCCESS;
}

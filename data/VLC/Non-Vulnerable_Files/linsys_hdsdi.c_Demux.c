 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    return ( Capture( p_demux ) == VLC_SUCCESS );
}

 *****************************************************************************/
static void StillTimer( void *p_data )
{
    demux_sys_t    *p_sys = p_data;
    vlc_mutex_lock( &p_sys->still.lock );
    if( likely(p_sys->still.b_enabled) )
    {
        p_sys->still.b_enabled = false;
        dvdnav_still_skip( p_sys->dvdnav );
    }
    vlc_mutex_unlock( &p_sys->still.lock );
}

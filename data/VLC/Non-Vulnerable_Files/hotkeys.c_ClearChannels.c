}
static void ClearChannels( intf_thread_t *p_intf, vout_thread_t *p_vout )
{
    if( p_vout )
    {
        vout_FlushSubpictureChannel( p_vout, SPU_DEFAULT_CHANNEL );
        vout_FlushSubpictureChannel( p_vout, p_intf->p_sys->slider_chan );
    }
}

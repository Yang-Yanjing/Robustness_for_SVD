}
static void AVCClose( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys->p_avc1394 )
    {
        raw1394_destroy_handle( p_sys->p_avc1394 );
        p_sys->p_avc1394 = NULL;
    }
}

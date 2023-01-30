 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    /* Stop data transmission */
    if( dc1394_video_set_transmission( p_sys->camera,
                       DC1394_OFF ) != DC1394_SUCCESS )
        msg_Err( p_demux, "Unable to stop camera iso transmission" );
    /* Close camera */
    dc1394_capture_stop( p_sys->camera );
    dc1394_camera_free(p_sys->camera);
    dc1394_free(p_sys->p_dccontext);
    free( p_sys->video_device );
    free( p_sys );
}

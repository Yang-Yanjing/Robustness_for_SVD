 *****************************************************************************/
void ioctl_Close( vlc_object_t * p_this, vcddev_t *p_vcddev )
{
    free( p_vcddev->psz_dev );
    if( p_vcddev->i_vcdimage_handle != -1 )
    {
        /*
         *  vcd image mode
         */
        CloseVCDImage( p_this, p_vcddev );
        return;
    }
    /*
     *  vcd device mode
     */
#ifdef _WIN32
    if( p_vcddev->h_device_handle )
        CloseHandle( p_vcddev->h_device_handle );
#elif defined( __OS2__ )
    if( p_vcddev->hcd )
        DosClose( p_vcddev->hcd );
#else
    if( p_vcddev->i_device_handle != -1 )
        close( p_vcddev->i_device_handle );
#endif
    free( p_vcddev );
}

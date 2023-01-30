 ****************************************************************************/
static void CloseVCDImage( vlc_object_t * p_this, vcddev_t *p_vcddev )
{
    VLC_UNUSED( p_this );
    if( p_vcddev->i_vcdimage_handle != -1 )
        close( p_vcddev->i_vcdimage_handle );
    else
        return;
    free( p_vcddev->p_sectors );
}

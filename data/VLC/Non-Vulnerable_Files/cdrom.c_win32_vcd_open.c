 *****************************************************************************/
static int win32_vcd_open( vlc_object_t * p_this, const char *psz_dev,
                           vcddev_t *p_vcddev )
{
    /* Initializations */
    p_vcddev->h_device_handle = NULL;
    char psz_win32_drive[7];
    msg_Dbg( p_this, "using winNT/2K/XP ioctl layer" );
    sprintf( psz_win32_drive, "\\\\.\\%c:", psz_dev[0] );
    p_vcddev->h_device_handle = CreateFileA( psz_win32_drive, GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            NULL, OPEN_EXISTING,
                                            FILE_FLAG_NO_BUFFERING |
                                            FILE_FLAG_RANDOM_ACCESS, NULL );
    return (p_vcddev->h_device_handle == NULL) ? -1 : 0;
}

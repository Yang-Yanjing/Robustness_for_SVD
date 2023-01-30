 *****************************************************************************/
vcddev_t *ioctl_Open( vlc_object_t *p_this, const char *psz_dev )
{
    int i_ret;
    int b_is_file;
    vcddev_t *p_vcddev;
#if !defined( _WIN32 ) && !defined( __OS2__ )
    struct stat fileinfo;
#endif
    if( !psz_dev ) return NULL;
    /*
     *  Initialize structure with default values
     */
    p_vcddev = malloc( sizeof(*p_vcddev) );
    if( p_vcddev == NULL )
        return NULL;
    p_vcddev->i_vcdimage_handle = -1;
    p_vcddev->psz_dev = NULL;
    b_is_file = 1;
    /*
     *  Check if we are dealing with a device or a file (vcd image)
     */
#if defined( _WIN32 ) || defined( __OS2__ )
    if( (strlen( psz_dev ) == 2 && psz_dev[1] == ':') )
    {
        b_is_file = 0;
    }
#else
    if( vlc_stat( psz_dev, &fileinfo ) < 0 )
    {
        free( p_vcddev );
        return NULL;
    }
    /* Check if this is a block/char device */
    if( S_ISBLK( fileinfo.st_mode ) || S_ISCHR( fileinfo.st_mode ) )
        b_is_file = 0;
#endif
    if( b_is_file )
    {
        i_ret = OpenVCDImage( p_this, psz_dev, p_vcddev );
    }
    else
    {
        /*
         *  open the vcd device
         */
#ifdef _WIN32
        i_ret = win32_vcd_open( p_this, psz_dev, p_vcddev );
#elif defined( __OS2__ )
        i_ret = os2_vcd_open( p_this, psz_dev, p_vcddev );
#else
        p_vcddev->i_device_handle = -1;
        p_vcddev->i_device_handle = vlc_open( psz_dev, O_RDONLY | O_NONBLOCK );
        i_ret = (p_vcddev->i_device_handle == -1) ? -1 : 0;
#endif
    }
    if( i_ret == 0 )
    {
        p_vcddev->psz_dev = (char *)strdup( psz_dev );
    }
    else
    {
        free( p_vcddev );
        p_vcddev = NULL;
    }
    return p_vcddev;
}

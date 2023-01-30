 *****************************************************************************/
static int ProbeDVD( const char *psz_name )
{
    if( !*psz_name )
        /* Triggers libdvdcss autodetection */
        return VLC_SUCCESS;
    int fd = vlc_open( psz_name, O_RDONLY | O_NONBLOCK );
    if( fd == -1 )
#ifdef HAVE_FDOPENDIR
        return VLC_EGENERIC;
#else
        return (errno == ENOENT) ? VLC_EGENERIC : VLC_SUCCESS;
#endif
    int ret = VLC_EGENERIC;
    struct stat stat_info;
    if( fstat( fd, &stat_info ) == -1 )
         goto bailout;
    if( !S_ISREG( stat_info.st_mode ) )
    {
        if( S_ISDIR( stat_info.st_mode ) || S_ISBLK( stat_info.st_mode ) )
            ret = VLC_SUCCESS; /* Let dvdnav_open() do the probing */
        goto bailout;
    }
    /* ISO 9660 volume descriptor */
    char iso_dsc[6];
    if( lseek( fd, 0x8000 + 1, SEEK_SET ) == -1
     || read( fd, iso_dsc, sizeof (iso_dsc) ) < sizeof (iso_dsc)
     || memcmp( iso_dsc, "CD001\x01", 6 ) )
        goto bailout;
    /* Try to find the anchor (2 bytes at LBA 256) */
    uint16_t anchor;
    if( lseek( fd, 256 * DVD_VIDEO_LB_LEN, SEEK_SET ) != -1
     && read( fd, &anchor, 2 ) == 2
     && GetWLE( &anchor ) == 2 )
        ret = VLC_SUCCESS; /* Found a potential anchor */
bailout:
    close( fd );
    return ret;
}

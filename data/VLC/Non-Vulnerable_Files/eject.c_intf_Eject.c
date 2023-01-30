 */
static int intf_Eject( vlc_object_t *p_this, const char *psz_device )
{
    VLC_UNUSED(p_this);
#if defined(_WIN32)
    MCI_OPEN_PARMS op;
    DWORD i_flags;
    TCHAR psz_drive[4];
    memset( &op, 0, sizeof(MCI_OPEN_PARMS) );
    op.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
    _tcscpy( psz_drive, TEXT("X:") );
    psz_drive[0] = psz_device[0];
    op.lpstrElementName = psz_drive;
    /* Set the flags for the device type */
    i_flags = MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID |
              MCI_OPEN_ELEMENT | MCI_OPEN_SHAREABLE;
    if( mciSendCommand( 0, MCI_OPEN, i_flags, (uintptr_t)&op ) )
        return VLC_EGENERIC;
    /* Eject disc */
    mciSendCommand( op.wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 0 );
    /* Release access to the device */
    mciSendCommand( op.wDeviceID, MCI_CLOSE, MCI_WAIT, 0 );
    return VLC_SUCCESS;
#elif defined (__linux__) || defined (HAVE_DVD_H)
    /* This code could be extended to support CD/DVD-ROM chargers */
    int fd = vlc_open( psz_device, O_RDONLY | O_NONBLOCK );
    if( fd == -1 )
    {
        msg_Err( p_this, "could not open device %s", psz_device );
        return VLC_EGENERIC;
    }
# if defined(__linux__)
    /* Try a simple ATAPI eject */
    if( ioctl( fd, CDROMEJECT, 0 ) < 0
     && EjectSCSI( fd ) )
# else
    if( ioctl( fd, CDROMEJECT, 0 ) < 0 )
# endif
    {
        msg_Err( p_this, "could not eject %s", psz_device );
        close( fd );
        return VLC_EGENERIC;
    }
    close( fd );
    return VLC_SUCCESS;
#else
    VLC_UNUSED( psz_device );
    msg_Warn( p_this, "CD-Rom ejection unsupported on this platform" );
    return VLC_EGENERIC;
#endif
}

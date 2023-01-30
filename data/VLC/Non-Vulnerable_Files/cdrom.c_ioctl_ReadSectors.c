 ****************************************************************************/
int ioctl_ReadSectors( vlc_object_t *p_this, const vcddev_t *p_vcddev,
                       int i_sector, uint8_t *p_buffer, int i_nb, int i_type )
{
    uint8_t *p_block;
    int i;
    if( i_type == VCD_TYPE )
        p_block = malloc( VCD_SECTOR_SIZE * i_nb );
    else
        p_block = p_buffer;
    if( p_vcddev->i_vcdimage_handle != -1 )
    {
        /*
         *  vcd image mode
         */
        if( lseek( p_vcddev->i_vcdimage_handle, i_sector * VCD_SECTOR_SIZE,
                   SEEK_SET ) == -1 )
        {
            msg_Err( p_this, "Could not lseek to sector %d", i_sector );
            goto error;
        }
        if( read( p_vcddev->i_vcdimage_handle, p_block, VCD_SECTOR_SIZE * i_nb)
            == -1 )
        {
            msg_Err( p_this, "Could not read sector %d", i_sector );
            goto error;
        }
    }
    else
    {
        /*
         *  vcd device mode
         */
#if defined( __APPLE__ )
        dk_cd_read_t cd_read;
        memset( &cd_read, 0, sizeof(cd_read) );
        cd_read.offset = i_sector * VCD_SECTOR_SIZE;
        cd_read.sectorArea = kCDSectorAreaSync | kCDSectorAreaHeader |
                             kCDSectorAreaSubHeader | kCDSectorAreaUser |
                             kCDSectorAreaAuxiliary;
        cd_read.sectorType = kCDSectorTypeUnknown;
        cd_read.buffer = p_block;
        cd_read.bufferLength = VCD_SECTOR_SIZE * i_nb;
        if( ioctl( p_vcddev->i_device_handle, DKIOCCDREAD, &cd_read ) == -1 )
        {
            msg_Err( p_this, "could not read block %d", i_sector );
            goto error;
        }
#elif defined( _WIN32 )
        DWORD dwBytesReturned;
        RAW_READ_INFO cdrom_raw;
        /* Initialize CDROM_RAW_READ structure */
        cdrom_raw.DiskOffset.QuadPart = CD_SECTOR_SIZE * i_sector;
        cdrom_raw.SectorCount = i_nb;
        cdrom_raw.TrackMode =  i_type == VCD_TYPE ? XAForm2 : CDDA;
        if( DeviceIoControl( p_vcddev->h_device_handle, IOCTL_CDROM_RAW_READ,
                             &cdrom_raw, sizeof(RAW_READ_INFO), p_block,
                             VCD_SECTOR_SIZE * i_nb, &dwBytesReturned,
                             NULL ) == 0 )
        {
            if( i_type == VCD_TYPE )
            {
                /* Retry in YellowMode2 */
                cdrom_raw.TrackMode = YellowMode2;
                if( DeviceIoControl( p_vcddev->h_device_handle,
                                     IOCTL_CDROM_RAW_READ, &cdrom_raw,
                                     sizeof(RAW_READ_INFO), p_block,
                                     VCD_SECTOR_SIZE * i_nb, &dwBytesReturned,
                                     NULL ) == 0 )
                    goto error;
            }
            else return -1;
        }
#elif defined( __OS2__ )
        cdrom_readlong_t readlong = {{'C', 'D', '0', '1'}, };
        ULONG param_len;
        ULONG data_len;
        ULONG rc;
        readlong.addr_mode = 0;         /* LBA mode */
        readlong.sectors   = i_nb;
        readlong.start     = i_sector;
        rc = DosDevIOCtl( p_vcddev->hcd, IOCTL_CDROMDISK, CDROMDISK_READLONG,
                          &readlong, sizeof( readlong ), &param_len,
                          p_block, VCD_SECTOR_SIZE * i_nb, &data_len );
        if( rc )
        {
            msg_Err( p_this, "could not read block %d", i_sector );
            goto error;
        }
#elif defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
        struct scsireq  sc;
        int i_ret;
        memset( &sc, 0, sizeof(sc) );
        sc.cmd[0] = 0xBE;
        sc.cmd[1] = i_type == VCD_TYPE ? SECTOR_TYPE_MODE2_FORM2:
                                         SECTOR_TYPE_CDDA;
        sc.cmd[2] = (i_sector >> 24) & 0xff;
        sc.cmd[3] = (i_sector >> 16) & 0xff;
        sc.cmd[4] = (i_sector >>  8) & 0xff;
        sc.cmd[5] = (i_sector >>  0) & 0xff;
        sc.cmd[6] = (i_nb >> 16) & 0xff;
        sc.cmd[7] = (i_nb >>  8) & 0xff;
        sc.cmd[8] = (i_nb      ) & 0xff;
        sc.cmd[9] = i_type == VCD_TYPE ? READ_CD_RAW_MODE2 : READ_CD_USERDATA;
        sc.cmd[10] = 0; /* sub channel */
        sc.cmdlen = 12;
        sc.databuf = (caddr_t)p_block;
        sc.datalen = VCD_SECTOR_SIZE * i_nb;
        sc.senselen = sizeof( sc.sense );
        sc.flags = SCCMD_READ;
        sc.timeout = 10000;
        i_ret = ioctl( p_vcddev->i_device_handle, SCIOCCOMMAND, &sc );
        if( i_ret == -1 )
        {
            msg_Err( p_this, "SCIOCCOMMAND failed" );
            goto error;
        }
        if( sc.retsts || sc.error )
        {
            msg_Err( p_this, "SCSI command failed: status %d error %d",
                             sc.retsts, sc.error );
            goto error;
        }
#elif defined( HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H )
        int i_size = VCD_SECTOR_SIZE;
        if( ioctl( p_vcddev->i_device_handle, CDRIOCSETBLOCKSIZE, &i_size )
            == -1 )
        {
            msg_Err( p_this, "Could not set block size" );
            goto error;
        }
        if( lseek( p_vcddev->i_device_handle,
                   i_sector * VCD_SECTOR_SIZE, SEEK_SET ) == -1 )
        {
            msg_Err( p_this, "Could not lseek to sector %d", i_sector );
            goto error;
        }
        if( read( p_vcddev->i_device_handle,
                  p_block, VCD_SECTOR_SIZE * i_nb ) == -1 )
        {
            msg_Err( p_this, "Could not read sector %d", i_sector );
            goto error;
        }
#else
        for( i = 0; i < i_nb; i++ )
        {
            int i_dummy = i_sector + i + 2 * CD_FRAMES;
#define p_msf ((struct cdrom_msf0 *)(p_block + i * VCD_SECTOR_SIZE))
            p_msf->minute =   i_dummy / (CD_FRAMES * CD_SECS);
            p_msf->second = ( i_dummy % (CD_FRAMES * CD_SECS) ) / CD_FRAMES;
            p_msf->frame =  ( i_dummy % (CD_FRAMES * CD_SECS) ) % CD_FRAMES;
#undef p_msf
            if( ioctl( p_vcddev->i_device_handle, CDROMREADRAW,
                       p_block + i * VCD_SECTOR_SIZE ) == -1 )
            {
                msg_Err( p_this, "could not read block %i from disc",
                         i_sector );
                if( i == 0 )
                    goto error;
                else
                    break;
            }
        }
#endif
    }
    /* For VCDs, we don't want to keep the header and footer of the
     * sectors read */
    if( i_type == VCD_TYPE )
    {
        for( i = 0; i < i_nb; i++ )
        {
            memcpy( p_buffer + i * VCD_DATA_SIZE,
                    p_block + i * VCD_SECTOR_SIZE + VCD_DATA_START,
                    VCD_DATA_SIZE );
        }
        free( p_block );
    }
    return( 0 );
error:
    if( i_type == VCD_TYPE )
        free( p_block );
    return( -1 );
}

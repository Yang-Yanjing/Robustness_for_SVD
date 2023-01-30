 *****************************************************************************/
int ioctl_GetTracksMap( vlc_object_t *p_this, const vcddev_t *p_vcddev,
                        int **pp_sectors )
{
    int i_tracks = 0;
    if( p_vcddev->i_vcdimage_handle != -1 )
    {
        /*
         *  vcd image mode
         */
        i_tracks = p_vcddev->i_tracks;
        if( pp_sectors )
        {
            *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
            if( *pp_sectors == NULL )
                return 0;
            memcpy( *pp_sectors, p_vcddev->p_sectors,
                    (i_tracks + 1) * sizeof(**pp_sectors) );
        }
        return i_tracks;
    }
    else
    {
        /*
         *  vcd device mode
         */
#if defined( __APPLE__ )
        CDTOC *pTOC;
        int i_descriptors;
        if( ( pTOC = darwin_getTOC( p_this, p_vcddev ) ) == NULL )
        {
            msg_Err( p_this, "failed to get the TOC" );
            return 0;
        }
        i_descriptors = CDTOCGetDescriptorCount( pTOC );
        i_tracks = darwin_getNumberOfTracks( pTOC, i_descriptors );
        if( pp_sectors )
        {
            int i, i_leadout = -1;
            CDTOCDescriptor *pTrackDescriptors;
            u_char track;
            *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
            if( *pp_sectors == NULL )
            {
                darwin_freeTOC( pTOC );
                return 0;
            }
            pTrackDescriptors = pTOC->descriptors;
            for( i_tracks = 0, i = 0; i < i_descriptors; i++ )
            {
                track = pTrackDescriptors[i].point;
                if( track == 0xA2 )
                    i_leadout = i;
                if( track > CD_MAX_TRACK_NO || track < CD_MIN_TRACK_NO )
                    continue;
                (*pp_sectors)[i_tracks++] =
                    CDConvertMSFToLBA( pTrackDescriptors[i].p );
            }
            if( i_leadout == -1 )
            {
                msg_Err( p_this, "leadout not found" );
                free( *pp_sectors );
                darwin_freeTOC( pTOC );
                return 0;
            }
            /* set leadout sector */
            (*pp_sectors)[i_tracks] =
                CDConvertMSFToLBA( pTrackDescriptors[i_leadout].p );
        }
        darwin_freeTOC( pTOC );
#elif defined( _WIN32 )
        DWORD dwBytesReturned;
        CDROM_TOC cdrom_toc;
        if( DeviceIoControl( p_vcddev->h_device_handle, IOCTL_CDROM_READ_TOC,
                             NULL, 0, &cdrom_toc, sizeof(CDROM_TOC),
                             &dwBytesReturned, NULL ) == 0 )
        {
            msg_Err( p_this, "could not read TOCHDR" );
            return 0;
        }
        i_tracks = cdrom_toc.LastTrack - cdrom_toc.FirstTrack + 1;
        if( pp_sectors )
        {
            *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
            if( *pp_sectors == NULL )
                return 0;
            for( int i = 0 ; i <= i_tracks ; i++ )
            {
                (*pp_sectors)[ i ] = MSF_TO_LBA2(
                                           cdrom_toc.TrackData[i].Address[1],
                                           cdrom_toc.TrackData[i].Address[2],
                                           cdrom_toc.TrackData[i].Address[3] );
                msg_Dbg( p_this, "p_sectors: %i, %i", i, (*pp_sectors)[i]);
             }
        }
#elif defined( __OS2__ )
        cdrom_get_tochdr_t get_tochdr = {{'C', 'D', '0', '1'}};
        cdrom_tochdr_t     tochdr;
        ULONG param_len;
        ULONG data_len;
        ULONG rc;
        rc = DosDevIOCtl( p_vcddev->hcd, IOCTL_CDROMAUDIO,
                          CDROMAUDIO_GETAUDIODISK,
                          &get_tochdr, sizeof( get_tochdr ), &param_len,
                          &tochdr, sizeof( tochdr ), &data_len );
        if( rc )
        {
            msg_Err( p_this, "could not read TOCHDR" );
            return 0;
        }
        i_tracks = tochdr.last_track - tochdr.first_track + 1;
        if( pp_sectors )
        {
            cdrom_get_track_t get_track = {{'C', 'D', '0', '1'}, };
            cdrom_track_t track;
            int i;
            *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
            if( *pp_sectors == NULL )
                return 0;
            for( i = 0 ; i < i_tracks ; i++ )
            {
                get_track.track = tochdr.first_track + i;
                rc = DosDevIOCtl( p_vcddev->hcd, IOCTL_CDROMAUDIO,
                                  CDROMAUDIO_GETAUDIOTRACK,
                                  &get_track, sizeof(get_track), &param_len,
                                  &track, sizeof(track), &data_len );
                if (rc)
                {
                    msg_Err( p_this, "could not read %d track",
                             get_track.track );
                    return 0;
                }
                (*pp_sectors)[ i ] = MSF_TO_LBA2(
                                       track.start.minute,
                                       track.start.second,
                                       track.start.frame );
                msg_Dbg( p_this, "p_sectors: %i, %i", i, (*pp_sectors)[i]);
            }
            /* for lead-out track */
            (*pp_sectors)[ i ] = MSF_TO_LBA2(
                                   tochdr.lead_out.minute,
                                   tochdr.lead_out.second,
                                   tochdr.lead_out.frame );
            msg_Dbg( p_this, "p_sectors: %i, %i", i, (*pp_sectors)[i]);
        }
#elif defined( HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H ) \
       || defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
        struct ioc_toc_header tochdr;
        struct ioc_read_toc_entry toc_entries;
        if( ioctl( p_vcddev->i_device_handle, CDIOREADTOCHEADER, &tochdr )
            == -1 )
        {
            msg_Err( p_this, "could not read TOCHDR" );
            return 0;
        }
        i_tracks = tochdr.ending_track - tochdr.starting_track + 1;
        if( pp_sectors )
        {
             int i;
             *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
             if( *pp_sectors == NULL )
                 return 0;
             toc_entries.address_format = CD_LBA_FORMAT;
             toc_entries.starting_track = 0;
             toc_entries.data_len = ( i_tracks + 1 ) *
                                        sizeof( struct cd_toc_entry );
             toc_entries.data = (struct cd_toc_entry *)
                                    malloc( toc_entries.data_len );
             if( toc_entries.data == NULL )
             {
                 free( *pp_sectors );
                 return 0;
             }
             /* Read the TOC */
             if( ioctl( p_vcddev->i_device_handle, CDIOREADTOCENTRYS,
                        &toc_entries ) == -1 )
             {
                 msg_Err( p_this, "could not read the TOC" );
                 free( *pp_sectors );
                 free( toc_entries.data );
                 return 0;
             }
             /* Fill the p_sectors structure with the track/sector matches */
             for( i = 0 ; i <= i_tracks ; i++ )
             {
#if defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
                 /* FIXME: is this ok? */
                 (*pp_sectors)[ i ] = toc_entries.data[i].addr.lba;
#else
                 (*pp_sectors)[ i ] = ntohl( toc_entries.data[i].addr.lba );
#endif
             }
        }
#else
        struct cdrom_tochdr   tochdr;
        struct cdrom_tocentry tocent;
        /* First we read the TOC header */
        if( ioctl( p_vcddev->i_device_handle, CDROMREADTOCHDR, &tochdr )
            == -1 )
        {
            msg_Err( p_this, "could not read TOCHDR" );
            return 0;
        }
        i_tracks = tochdr.cdth_trk1 - tochdr.cdth_trk0 + 1;
        if( pp_sectors )
        {
            int i;
            *pp_sectors = calloc( i_tracks + 1, sizeof(**pp_sectors) );
            if( *pp_sectors == NULL )
                return 0;
            /* Fill the p_sectors structure with the track/sector matches */
            for( i = 0 ; i <= i_tracks ; i++ )
            {
                tocent.cdte_format = CDROM_LBA;
                tocent.cdte_track =
                    ( i == i_tracks ) ? CDROM_LEADOUT : tochdr.cdth_trk0 + i;
                if( ioctl( p_vcddev->i_device_handle, CDROMREADTOCENTRY,
                           &tocent ) == -1 )
                {
                    msg_Err( p_this, "could not read TOCENTRY" );
                    free( *pp_sectors );
                    return 0;
                }
                (*pp_sectors)[ i ] = tocent.cdte_addr.lba;
            }
        }
#endif
        return i_tracks;
    }
}

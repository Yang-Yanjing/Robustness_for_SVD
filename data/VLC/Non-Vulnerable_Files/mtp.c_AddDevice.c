 *****************************************************************************/
static int AddDevice( services_discovery_t *p_sd,
                      LIBMTP_raw_device_t *p_raw_device )
{
    char *psz_name = NULL;
    LIBMTP_mtpdevice_t *p_device;
    LIBMTP_track_t *p_track, *p_tmp;
    if( ( p_device = LIBMTP_Open_Raw_Device( p_raw_device ) ) != NULL )
    {
        if( !( psz_name = LIBMTP_Get_Friendlyname( p_device ) ) )
            if( !( psz_name = LIBMTP_Get_Modelname( p_device ) ) )
                if( !( psz_name = strdup( N_( "MTP Device" ) ) ) )
                    return VLC_ENOMEM;
        msg_Info( p_sd, "Found device: %s", psz_name );
        p_sd->p_sys->i_bus = p_raw_device->bus_location;
        p_sd->p_sys->i_dev = p_raw_device->devnum;
        p_sd->p_sys->i_product_id = p_raw_device->device_entry.product_id;
        if( ( p_track = LIBMTP_Get_Tracklisting_With_Callback( p_device,
                            CountTracks, p_sd ) ) == NULL )
        {
            msg_Warn( p_sd, "No tracks on the device" );
        }
        else
        {
            if( !( p_sd->p_sys->pp_items = calloc( p_sd->p_sys->i_tracks_num,
                                                   sizeof( input_item_t * ) ) ) )
            {
                free( psz_name );
                return VLC_ENOMEM;
            }
            p_sd->p_sys->i_count = 0;
            while( p_track != NULL )
            {
                msg_Dbg( p_sd, "Track found: %s - %s", p_track->artist,
                         p_track->title );
                AddTrack( p_sd, p_track );
                p_tmp = p_track;
                p_track = p_track->next;
                LIBMTP_destroy_track_t( p_tmp );
            }
        }
        p_sd->p_sys->psz_name = psz_name;
        LIBMTP_Release_Device( p_device );
        return VLC_SUCCESS;
    }
    else
    {
        msg_Info( p_sd, "The device seems to be mounted, unmount it first" );
        return VLC_EGENERIC;
    }
}

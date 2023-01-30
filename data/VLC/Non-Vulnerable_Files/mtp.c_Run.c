 *****************************************************************************/
static void *Run( void *data )
{
    LIBMTP_raw_device_t *p_rawdevices;
    int i_numrawdevices;
    int i_ret;
    int i_status = 0;
    services_discovery_t *p_sd = data;
    for(;;)
    {
        int canc = vlc_savecancel();
        i_ret = LIBMTP_Detect_Raw_Devices( &p_rawdevices, &i_numrawdevices );
        if ( i_ret == 0 && i_numrawdevices > 0 && p_rawdevices != NULL &&
             i_status == 0 )
        {
            /* Found a new device, add it */
            msg_Dbg( p_sd, "New device found" );
            if( AddDevice( p_sd, &p_rawdevices[0] ) == VLC_SUCCESS )
                i_status = 1;
            else
                i_status = 2;
        }
        else
        {
            if ( ( i_ret != 0 || i_numrawdevices == 0 || p_rawdevices == NULL )
                 && i_status == 1)
            {
                /* The device is not connected anymore, delete it */
                msg_Info( p_sd, "Device disconnected" );
                CloseDevice( p_sd );
                i_status = 0;
            }
        }
        free( p_rawdevices );
        vlc_restorecancel(canc);
        if( i_status == 2 )
        {
            msleep( 5000000 );
            i_status = 0;
        }
        else
            msleep( 500000 );
    }
    return NULL;
}

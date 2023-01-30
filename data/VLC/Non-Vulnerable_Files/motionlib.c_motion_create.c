};
motion_sensors_t *motion_create( vlc_object_t *obj )
{
    FILE *f;
    int i_x = 0, i_y = 0;
    motion_sensors_t *motion = malloc( sizeof( motion_sensors_t ) );
    if( unlikely( motion == NULL ) )
    {
        return NULL;
    }
    if( access( "/sys/devices/platform/hdaps/position", R_OK ) == 0 
        && ( f = fopen( "/sys/devices/platform/hdaps/calibrate", "re" ) ) )
    {
        /* IBM HDAPS support */
        motion->i_calibrate = fscanf( f, "(%d,%d)", &i_x, &i_y ) == 2 ? i_x: 0;
        fclose( f );
        motion->sensor = HDAPS_SENSOR;
        msg_Dbg( obj, "HDAPS motion detection correctly loaded" );
    }
    else if( access( "/sys/devices/ams/x", R_OK ) == 0 )
    {
        /* Apple Motion Sensor support */
        motion->sensor = AMS_SENSOR;
        msg_Dbg( obj, "AMS motion detection correctly loaded" );
    }
    else if( access( "/sys/devices/platform/applesmc.768/position", R_OK ) == 0 
             && ( f = fopen( "/sys/devices/platform/applesmc.768/calibrate", "re" ) ) )
    {
        /* Apple SMC (newer macbooks) */
        /* Should be factorised with HDAPS */
        motion->i_calibrate = fscanf( f, "(%d,%d)", &i_x, &i_y ) == 2 ? i_x: 0;
        fclose( f );
        motion->sensor = APPLESMC_SENSOR;
        msg_Dbg( obj, "Apple SMC motion detection correctly loaded" );
    }
#ifdef HAVE_MACOS_UNIMOTION
    else if( (motion->unimotion_hw = detect_sms()) )
    {
        motion->sensor = UNIMOTION_SENSOR;
        msg_Dbg( obj, "UniMotion motion detection correctly loaded" );
    }
#endif
    else
    {
        /* No motion sensor support */
        msg_Err( obj, "No motion sensor available" );
        free( motion );
        return NULL;
    }
    memset( motion->p_oldx, 0, sizeof( motion->p_oldx ) );
    motion->i = 0;
    motion->i_sum = 0;
    return motion;
}

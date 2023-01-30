 *****************************************************************************/
static int GetOrientation( motion_sensors_t *motion )
{
    FILE *f;
    int i_x = 0, i_y = 0, i_z = 0;
    int i_ret;
    switch( motion->sensor )
    {
    case HDAPS_SENSOR:
        f = fopen( "/sys/devices/platform/hdaps/position", "re" );
        if( !f )
        {
            return 0;
        }
        i_ret = fscanf( f, "(%d,%d)", &i_x, &i_y );
        fclose( f );
        if( i_ret < 2 )
            return 0;
        else
            return ( i_x - motion->i_calibrate ) * 10;
    case AMS_SENSOR:
        f = fopen( "/sys/devices/ams/x", "re" );
        if( !f )
        {
            return 0;
        }
        i_ret = fscanf( f, "%d", &i_x);
        fclose( f );
        if( i_ret < 1 )
            return 0;
        else
            return - i_x * 30; /* FIXME: arbitrary */
    case APPLESMC_SENSOR:
        f = fopen( "/sys/devices/platform/applesmc.768/position", "re" );
        if( !f )
        {
            return 0;
        }
        i_ret = fscanf( f, "(%d,%d,%d)", &i_x, &i_y, &i_z );
        fclose( f );
        if( i_ret < 3 )
            return 0;
        else
            return ( i_x - motion->i_calibrate ) * 10;
#ifdef HAVE_MACOS_UNIMOTION
    case UNIMOTION_SENSOR:
        if( read_sms_raw( motion->unimotion_hw, &i_x, &i_y, &i_z ) )
        {
            double d_norm = sqrt( i_x*i_x+i_z*i_z );
            if( d_norm < 100 )
                return 0;
            double d_x = i_x / d_norm;
            if( i_z > 0 )
                return -asin(d_x)*3600/3.141;
            else
                return 3600 + asin(d_x)*3600/3.141;
        }
        else
            return 0;
#endif
    default:
        assert( 0 );
    }
}

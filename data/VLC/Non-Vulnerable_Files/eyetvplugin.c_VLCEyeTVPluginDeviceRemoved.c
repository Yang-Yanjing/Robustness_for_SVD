/* called if a device is removed */
static long VLCEyeTVPluginDeviceRemoved(VLCEyeTVPluginGlobals_t *globals, EyeTVPluginDeviceID deviceID)
{
    printf("VLC media player Plug-In: DeviceRemoved\n");
    long result = 0;
    --i_deviceCount;
    if( 0 == i_deviceCount )
    {                
        /* notify a potential VLC instance about the removal */
        CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter(),
                                              CFSTR("DeviceRemoved"), 
                                              CFSTR(VLC_NOTIFICATION_OBJECT), 
                                              /*userInfo*/ NULL, 
                                              TRUE );
    }
    if( (i_vlcSock != -1) && (deviceID == globals->activeDeviceID) )
    {
        close(i_vlcSock);
        i_vlcSock = -1;
        printf( "data sending switched off\n" );
    }
    return result;
}

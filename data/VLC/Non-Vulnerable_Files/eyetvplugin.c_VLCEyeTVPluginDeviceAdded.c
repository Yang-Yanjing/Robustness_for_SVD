/* called if a device is added */
static long VLCEyeTVPluginDeviceAdded(VLCEyeTVPluginGlobals_t *globals, EyeTVPluginDeviceID deviceID, EyeTVPluginDeviceType deviceType)
{
    printf("VLC media player Plug-In: Device with type %i and ID %i added\n", (int)deviceType, (int)deviceID);
    long result = 0;
    if( globals ) 
    {
        ++i_deviceCount;
        if( 1 == i_deviceCount )
        {                
            /* notify a potential VLC instance about the addition */
            CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter(),
                                                  CFSTR("DeviceAdded"), 
                                                  CFSTR(VLC_NOTIFICATION_OBJECT), 
                                                  /*userInfo*/ NULL, 
                                                  TRUE );
        }
    }
    return result;
}

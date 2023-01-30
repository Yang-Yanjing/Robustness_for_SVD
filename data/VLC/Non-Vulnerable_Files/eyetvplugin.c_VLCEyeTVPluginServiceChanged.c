 */
static long VLCEyeTVPluginServiceChanged(VLCEyeTVPluginGlobals_t *globals, 
                                            EyeTVPluginDeviceID deviceID, 
                                            long headendID, 
                                            long transponderID, 
                                            long serviceID, 
                                            EyeTVPluginPIDInfo *pidList, 
                                            long pidsCount)
{
    long result = 0;
    int i;
    printf("\nVLC media player Plug-In: ServiceChanged:\n");
    printf(  "=====================================\n");
    if( globals ) 
    {
        printf("DeviceID: %ld, ", (long)deviceID);
        printf("HeadendID: %ld, ", headendID);
        printf("TransponderID: %ld, ", transponderID);
        printf("ServiceID: %ld\n\n", serviceID);
        globals->activeDeviceID = deviceID;
        globals->activePIDsCount = pidsCount;
        /* need active PIDs for packet filtering */
        for( i = 0; i < pidsCount; i++ )
        {
            globals->activePIDs[i] = pidList[i];
            printf("Active PID: %ld, type: %ld\n", pidList[i].pid, pidList[i].pidType);
        }
    }
    printf(  "=====================================\n");
    /* notify a potential VLC instance about the service change */
    CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter(),
                                          CFSTR("ServiceChanged"), 
                                          CFSTR(VLC_NOTIFICATION_OBJECT), 
                                          /*userInfo*/ NULL, 
                                          TRUE );
    return result;
}

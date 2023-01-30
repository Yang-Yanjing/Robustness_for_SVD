/* initialise the plug-in */
static long VLCEyeTVPluginInitialize(VLCEyeTVPluginGlobals_t** globals, long apiVersion, EyeTVPluginCallbackProc callback)
{
    printf("VLC media player Plug-In: Initialize\n");
    long result = 0;
    /* init our own storage */
    i_deviceCount = 0;
    i_vlcSock = -1;
    /* notify a potential VLC instance about our initialisation */
    CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter(),
                                          CFSTR("PluginInit"), 
                                          CFSTR(VLC_NOTIFICATION_OBJECT), 
                                          /*userInfo*/ NULL, 
                                          TRUE );
    /* init our notification support */
    CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(),
                                     /* observer */ NULL, 
                                     /* callBack */ VLCEyeTVPluginGlobalNotificationReceived,
                                     /* name, NULL==all */ NULL,
                                     CFSTR(VLC_NOTIFICATION_OBJECT), 
                                     CFNotificationSuspensionBehaviorDeliverImmediately );
    *globals = (VLCEyeTVPluginGlobals_t *) calloc(1, sizeof( VLCEyeTVPluginGlobals_t ) );
    ( *globals )->callback = callback;
    return result;
}

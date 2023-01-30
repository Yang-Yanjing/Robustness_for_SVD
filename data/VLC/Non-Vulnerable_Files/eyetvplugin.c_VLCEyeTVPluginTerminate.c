/* we will be terminated soon, clean up */
static long VLCEyeTVPluginTerminate(VLCEyeTVPluginGlobals_t *globals)
{
    long result = 0;
    printf("VLC media player Plug-In: Terminate\n");
    /* notify a potential VLC instance about our termination */
    CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter (),
                                          CFSTR("PluginQuit"), 
                                          CFSTR(VLC_NOTIFICATION_OBJECT), 
                                          /*userInfo*/ NULL, 
                                          TRUE );
    /* remove us from the global notification centre */
    CFNotificationCenterRemoveEveryObserver( CFNotificationCenterGetDistributedCenter(),
                                             (void *)VLCEyeTVPluginGlobalNotificationReceived );
    /* close data connection */
    if( i_vlcSock != -1 )
    {
        close( i_vlcSock );
        i_vlcSock = -1;
    }
    free( globals );
    return result;
}

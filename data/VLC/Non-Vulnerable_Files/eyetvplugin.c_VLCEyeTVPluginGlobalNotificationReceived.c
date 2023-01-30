/* called if we received a global notification */
void VLCEyeTVPluginGlobalNotificationReceived( CFNotificationCenterRef center, 
                                              void *observer, 
                                              CFStringRef name, 
                                              const void *object, 
                                              CFDictionaryRef userInfo )
{
    /* when VLC launches after us, we need to inform it about our existance and the current state of available devices */
    if( CFStringCompare( name, CFSTR( "VLCOSXGUIInit" ), 0) == kCFCompareEqualTo )
    {
        /* we're here */
        CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter (),
                                              CFSTR("PluginInit"), 
                                              CFSTR(VLC_NOTIFICATION_OBJECT), 
                                              /*userInfo*/ NULL, 
                                              TRUE );
        if( i_deviceCount > 0 )
        {
            /* at least one device is apparently connected */
            CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter (),
                                                  CFSTR("DeviceAdded"), 
                                                  CFSTR(VLC_NOTIFICATION_OBJECT), 
                                                  /*userInfo*/ NULL, 
                                                  TRUE );
        }
    }
    /* VLC wants us to start sending data */
    if( CFStringCompare( name, CFSTR( "VLCAccessStartDataSending" ), 0) == kCFCompareEqualTo )
    {
        if( i_vlcSock == -1 )
        {
            int peerSock;
            /* set-up data socket */
            peerSock = socket(AF_UNIX, SOCK_STREAM, 0);
            if( peerSock != -1 )
            {
                struct sockaddr_un peerAddr;
                /* set-up connection address */
                memset(&peerAddr, 0, sizeof(peerAddr));
                peerAddr.sun_family = AF_UNIX;
                strncpy(peerAddr.sun_path, "/tmp/.vlc-eyetv-bridge", sizeof(peerAddr.sun_path)-1);
                /* connect */
                printf("data connect in progess...\n");
                if( connect(peerSock, (struct sockaddr *)&peerAddr, sizeof(struct sockaddr_un)) != -1 )
                {
                    printf("data sending switched on\n");
                    i_vlcSock = peerSock;
                }
                else
                    printf("connect data socket failed (errno=%d)\n", errno );
            }
            else
                printf("create data socket failed (errno=%d)\n", errno );
        }
    }
    /* VLC wants us to stop sending data */
    if( CFStringCompare( name, CFSTR( "VLCAccessStopDataSending" ), 0) == kCFCompareEqualTo )
    {
        if( i_vlcSock != -1 )
        {
            close( i_vlcSock );
            i_vlcSock = -1;
            printf( "data sending switched off\n" );
        }
    }
}

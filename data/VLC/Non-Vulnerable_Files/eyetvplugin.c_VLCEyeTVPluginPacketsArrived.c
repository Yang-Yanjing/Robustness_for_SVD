 * finished. Therefore all in this method should be as fast as possible. */
static long VLCEyeTVPluginPacketsArrived(VLCEyeTVPluginGlobals_t *globals, EyeTVPluginDeviceID deviceID, long **packets, long packetsCount)
{
    if( globals ) 
    {
        /* check if data connection is active */
        if( i_vlcSock != -1 )
        {
            if( deviceID == globals->activeDeviceID ) 
            {
                long pidCount = globals->activePIDsCount;
                if( pidCount )
                {
                    uint8_t packetBuffer[sizeof(TransportStreamPacket)*20];
                    int packetBufferSize = 0;
                    while( packetsCount )
                    {
                        /* apply PID filtering, only PIDs in active service for device are sent through */
                        long pid = ntohl(**packets)>>8 & 0x1FFFL;
                        /* ignore NULL packets */
                        if( 0x1FFFL != pid )
                        {
                            long i;
                            for( i=0; i<pidCount; ++i )
                            {
                                if( globals->activePIDs[i].pid == pid )
                                {
                                    memcpy(packetBuffer+packetBufferSize, *packets, sizeof(TransportStreamPacket));
                                    packetBufferSize += sizeof(TransportStreamPacket);
                                    if( packetBufferSize > (sizeof(packetBuffer)-sizeof(TransportStreamPacket)) )
                                    {
                                        /* flush buffer to VLC */
                                        ssize_t sent = write(i_vlcSock, packetBuffer, packetBufferSize);
                                        if( sent != packetBufferSize )
                                        {
                                            if( sent == -1 )
                                                printf("data sending failed (errno=%d)\n", errno);
                                            else
                                                printf("data sending incomplete (sent=%zd)\n", sent);
                                            close(i_vlcSock);
                                            i_vlcSock = -1;
                                            return 0;
                                        }
                                        packetBufferSize = 0;
                                    }
                                    if( i > 0 )
                                    {
                                       /* if we assume that consecutive packets would have the same PID in most cases,
                                          it would therefore speed up filtering to reorder activePIDs list based on pid
                                          occurrences */
                                        EyeTVPluginPIDInfo swap = globals->activePIDs[i];
                                        do
                                        {
                                            register int c = i--;
                                            globals->activePIDs[c] = globals->activePIDs[i];
                                        }
                                        while( i );
                                        globals->activePIDs[i] = swap;
                                    }
                                    if( pid && globals->activePIDs[0].pidType != kEyeTVPIDType_PMT )
                                    {
                                        /* to save on CPU, prevent EyeTV from mirroring that program by blocking video & audio packets
                                           by changing all packets but PAT and PMT to NULL PID */
#if defined(WORDS_BIGENDIAN)
                                        **packets |= 0x001FFF00L;
#else
                                        **packets |= 0x00FFF800L;
#endif
                                    }
                                    /* done filtering on this packet, move on to next packet */
                                    break;
                                }
                            }
                        }
                        --packetsCount;
                        ++packets;
                    }
                    if( packetBufferSize )
                    {
                        /* flush buffer to VLC */
                        ssize_t sent = write(i_vlcSock, packetBuffer, packetBufferSize);
                        if( sent != packetBufferSize )
                        {
                            if( sent == -1 )
                                printf("data sending failed (errno=%d)\n", errno);
                            else
                                printf("data sending incomplete (sent=%zd)\n", sent);
                            close(i_vlcSock);
                            i_vlcSock = -1;
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

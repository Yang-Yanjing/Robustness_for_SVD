static int rtmp_handshake(URLContext *s, RTMPContext *rt)
{
    AVLFG rnd;
    uint8_t tosend    [RTMP_HANDSHAKE_PACKET_SIZE+1] = {
        3,                
        0, 0, 0, 0,       
        RTMP_CLIENT_VER1,
        RTMP_CLIENT_VER2,
        RTMP_CLIENT_VER3,
        RTMP_CLIENT_VER4,
    };
    uint8_t clientdata[RTMP_HANDSHAKE_PACKET_SIZE];
    uint8_t serverdata[RTMP_HANDSHAKE_PACKET_SIZE+1];
    int i;
    int server_pos, client_pos;
    uint8_t digest[32], signature[32];
    int ret, type = 0;
    av_log(s, AV_LOG_DEBUG, "Handshaking...\n");
    av_lfg_init(&rnd, 0xDEADC0DE);
    
    for (i = 9; i <= RTMP_HANDSHAKE_PACKET_SIZE; i++)
        tosend[i] = av_lfg_get(&rnd) >> 24;
    if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
        


        tosend[0] = 6;
        tosend[5] = 128;
        tosend[6] = 0;
        tosend[7] = 3;
        tosend[8] = 2;
        

        if ((ret = ff_rtmpe_gen_pub_key(rt->stream, tosend + 1)) < 0)
            return ret;
    }
    client_pos = rtmp_handshake_imprint_with_digest(tosend + 1, rt->encrypted);
    if (client_pos < 0)
        return client_pos;
    if ((ret = ffurl_write(rt->stream, tosend,
                           RTMP_HANDSHAKE_PACKET_SIZE + 1)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot write RTMP handshake request\n");
        return ret;
    }
    if ((ret = ffurl_read_complete(rt->stream, serverdata,
                                   RTMP_HANDSHAKE_PACKET_SIZE + 1)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot read RTMP handshake response\n");
        return ret;
    }
    if ((ret = ffurl_read_complete(rt->stream, clientdata,
                                   RTMP_HANDSHAKE_PACKET_SIZE)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot read RTMP handshake response\n");
        return ret;
    }
    av_log(s, AV_LOG_DEBUG, "Type answer %d\n", serverdata[0]);
    av_log(s, AV_LOG_DEBUG, "Server version %d.%d.%d.%d\n",
           serverdata[5], serverdata[6], serverdata[7], serverdata[8]);
    if (rt->is_input && serverdata[5] >= 3) {
        server_pos = rtmp_validate_digest(serverdata + 1, 772);
        if (server_pos < 0)
            return server_pos;
        if (!server_pos) {
            type = 1;
            server_pos = rtmp_validate_digest(serverdata + 1, 8);
            if (server_pos < 0)
                return server_pos;
            if (!server_pos) {
                av_log(s, AV_LOG_ERROR, "Server response validating failed\n");
                return AVERROR(EIO);
            }
        }
        

        if (rt->swfsize) {
            if ((ret = rtmp_calc_swf_verification(s, rt, serverdata + 1 +
                                                  RTMP_HANDSHAKE_PACKET_SIZE - 32)) < 0)
                return ret;
        }
        ret = ff_rtmp_calc_digest(tosend + 1 + client_pos, 32, 0,
                                  rtmp_server_key, sizeof(rtmp_server_key),
                                  digest);
        if (ret < 0)
            return ret;
        ret = ff_rtmp_calc_digest(clientdata, RTMP_HANDSHAKE_PACKET_SIZE - 32,
                                  0, digest, 32, signature);
        if (ret < 0)
            return ret;
        if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
            

            if ((ret = ff_rtmpe_compute_secret_key(rt->stream, serverdata + 1,
                                                   tosend + 1, type)) < 0)
                return ret;
            
            ff_rtmpe_encrypt_sig(rt->stream, signature, digest, serverdata[0]);
        }
        if (memcmp(signature, clientdata + RTMP_HANDSHAKE_PACKET_SIZE - 32, 32)) {
            av_log(s, AV_LOG_ERROR, "Signature mismatch\n");
            return AVERROR(EIO);
        }
        for (i = 0; i < RTMP_HANDSHAKE_PACKET_SIZE; i++)
            tosend[i] = av_lfg_get(&rnd) >> 24;
        ret = ff_rtmp_calc_digest(serverdata + 1 + server_pos, 32, 0,
                                  rtmp_player_key, sizeof(rtmp_player_key),
                                  digest);
        if (ret < 0)
            return ret;
        ret = ff_rtmp_calc_digest(tosend, RTMP_HANDSHAKE_PACKET_SIZE - 32, 0,
                                  digest, 32,
                                  tosend + RTMP_HANDSHAKE_PACKET_SIZE - 32);
        if (ret < 0)
            return ret;
        if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
            
            ff_rtmpe_encrypt_sig(rt->stream, tosend +
                                 RTMP_HANDSHAKE_PACKET_SIZE - 32, digest,
                                 serverdata[0]);
        }
        
        if ((ret = ffurl_write(rt->stream, tosend,
                               RTMP_HANDSHAKE_PACKET_SIZE)) < 0)
            return ret;
        if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
            
            if ((ret = ff_rtmpe_update_keystream(rt->stream)) < 0)
                return ret;
        }
    } else {
        if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
            

            if ((ret = ff_rtmpe_compute_secret_key(rt->stream, serverdata + 1,
                            tosend + 1, 1)) < 0)
                return ret;
            if (serverdata[0] == 9) {
                
                ff_rtmpe_encrypt_sig(rt->stream, signature, digest,
                                     serverdata[0]);
            }
        }
        if ((ret = ffurl_write(rt->stream, serverdata + 1,
                               RTMP_HANDSHAKE_PACKET_SIZE)) < 0)
            return ret;
        if (CONFIG_FFRTMPCRYPT_PROTOCOL && rt->encrypted) {
            
            if ((ret = ff_rtmpe_update_keystream(rt->stream)) < 0)
                return ret;
        }
    }
    return 0;
}

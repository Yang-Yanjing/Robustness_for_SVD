static int rtmp_open(URLContext *s, const char *uri, int flags)
{
    RTMPContext *rt = s->priv_data;
    char proto[8], hostname[256], path[1024], auth[100], *fname;
    char *old_app, *qmark, *n, fname_buffer[1024];
    uint8_t buf[2048];
    int port;
    AVDictionary *opts = NULL;
    int ret;
    if (rt->listen_timeout > 0)
        rt->listen = 1;
    rt->is_input = !(flags & AVIO_FLAG_WRITE);
    av_url_split(proto, sizeof(proto), auth, sizeof(auth),
                 hostname, sizeof(hostname), &port,
                 path, sizeof(path), s->filename);
    n = strchr(path, ' ');
    if (n) {
        av_log(s, AV_LOG_WARNING,
               "Detected librtmp style URL parameters, these aren't supported "
               "by the libavformat internal RTMP handler currently enabled. "
               "See the documentation for the correct way to pass parameters.\n");
        *n = '\0'; 
    }
    if (auth[0]) {
        char *ptr = strchr(auth, ':');
        if (ptr) {
            *ptr = '\0';
            av_strlcpy(rt->username, auth, sizeof(rt->username));
            av_strlcpy(rt->password, ptr + 1, sizeof(rt->password));
        }
    }
    if (rt->listen && strcmp(proto, "rtmp")) {
        av_log(s, AV_LOG_ERROR, "rtmp_listen not available for %s\n",
               proto);
        return AVERROR(EINVAL);
    }
    if (!strcmp(proto, "rtmpt") || !strcmp(proto, "rtmpts")) {
        if (!strcmp(proto, "rtmpts"))
            av_dict_set(&opts, "ffrtmphttp_tls", "1", 1);
        
        ff_url_join(buf, sizeof(buf), "ffrtmphttp", NULL, hostname, port, NULL);
    } else if (!strcmp(proto, "rtmps")) {
        
        if (port < 0)
            port = RTMPS_DEFAULT_PORT;
        ff_url_join(buf, sizeof(buf), "tls", NULL, hostname, port, NULL);
    } else if (!strcmp(proto, "rtmpe") || (!strcmp(proto, "rtmpte"))) {
        if (!strcmp(proto, "rtmpte"))
            av_dict_set(&opts, "ffrtmpcrypt_tunneling", "1", 1);
        
        ff_url_join(buf, sizeof(buf), "ffrtmpcrypt", NULL, hostname, port, NULL);
        rt->encrypted = 1;
    } else {
        
        if (port < 0)
            port = RTMP_DEFAULT_PORT;
        if (rt->listen)
            ff_url_join(buf, sizeof(buf), "tcp", NULL, hostname, port,
                        "?listen&listen_timeout=%d",
                        rt->listen_timeout * 1000);
        else
            ff_url_join(buf, sizeof(buf), "tcp", NULL, hostname, port, NULL);
    }
reconnect:
    if ((ret = ffurl_open(&rt->stream, buf, AVIO_FLAG_READ_WRITE,
                          &s->interrupt_callback, &opts)) < 0) {
        av_log(s , AV_LOG_ERROR, "Cannot open connection %s\n", buf);
        goto fail;
    }
    if (rt->swfverify) {
        if ((ret = rtmp_calc_swfhash(s)) < 0)
            goto fail;
    }
    rt->state = STATE_START;
    if (!rt->listen && (ret = rtmp_handshake(s, rt)) < 0)
        goto fail;
    if (rt->listen && (ret = rtmp_server_handshake(s, rt)) < 0)
        goto fail;
    rt->out_chunk_size = 128;
    rt->in_chunk_size  = 128; 
    rt->state = STATE_HANDSHAKED;
    
    old_app = rt->app;
    rt->app = av_malloc(APP_MAX_LENGTH);
    if (!rt->app) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    
    qmark = strchr(path, '?');
    if (qmark && strstr(qmark, "slist=")) {
        char* amp;
        
        av_strlcpy(rt->app, path + 1, FFMIN(qmark - path, APP_MAX_LENGTH));
        fname = strstr(path, "slist=") + 6;
        
        amp = strchr(fname, '&');
        if (amp) {
            av_strlcpy(fname_buffer, fname, FFMIN(amp - fname + 1,
                                                  sizeof(fname_buffer)));
            fname = fname_buffer;
        }
    } else if (!strncmp(path, "/ondemand/", 10)) {
        fname = path + 10;
        memcpy(rt->app, "ondemand", 9);
    } else {
        char *next = *path ? path + 1 : path;
        char *p = strchr(next, '/');
        if (!p) {
            if (old_app) {
                
                
                fname = next;
            } else {
                fname = NULL;
                av_strlcpy(rt->app, next, APP_MAX_LENGTH);
            }
        } else {
            
            char *c = strchr(p + 1, ':');
            fname = strchr(p + 1, '/');
            if (!fname || (c && c < fname)) {
                fname = p + 1;
                av_strlcpy(rt->app, path + 1, FFMIN(p - path, APP_MAX_LENGTH));
            } else {
                fname++;
                av_strlcpy(rt->app, path + 1, FFMIN(fname - path - 1, APP_MAX_LENGTH));
            }
        }
    }
    if (old_app) {
        
        if (strlen(old_app) >= APP_MAX_LENGTH) {
            ret = AVERROR(EINVAL);
            goto fail;
        }
        av_free(rt->app);
        rt->app = old_app;
    }
    if (!rt->playpath) {
        rt->playpath = av_malloc(PLAYPATH_MAX_LENGTH);
        if (!rt->playpath) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if (fname) {
            int len = strlen(fname);
            if (!strchr(fname, ':') && len >= 4 &&
                (!strcmp(fname + len - 4, ".f4v") ||
                 !strcmp(fname + len - 4, ".mp4"))) {
                memcpy(rt->playpath, "mp4:", 5);
            } else {
                if (len >= 4 && !strcmp(fname + len - 4, ".flv"))
                    fname[len - 4] = '\0';
                rt->playpath[0] = 0;
            }
            av_strlcat(rt->playpath, fname, PLAYPATH_MAX_LENGTH);
        } else {
            rt->playpath[0] = '\0';
        }
    }
    if (!rt->tcurl) {
        rt->tcurl = av_malloc(TCURL_MAX_LENGTH);
        if (!rt->tcurl) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        ff_url_join(rt->tcurl, TCURL_MAX_LENGTH, proto, NULL, hostname,
                    port, "/%s", rt->app);
    }
    if (!rt->flashver) {
        rt->flashver = av_malloc(FLASHVER_MAX_LENGTH);
        if (!rt->flashver) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if (rt->is_input) {
            snprintf(rt->flashver, FLASHVER_MAX_LENGTH, "%s %d,%d,%d,%d",
                    RTMP_CLIENT_PLATFORM, RTMP_CLIENT_VER1, RTMP_CLIENT_VER2,
                    RTMP_CLIENT_VER3, RTMP_CLIENT_VER4);
        } else {
            snprintf(rt->flashver, FLASHVER_MAX_LENGTH,
                    "FMLE/3.0 (compatible; %s)", LIBAVFORMAT_IDENT);
        }
    }
    rt->client_report_size = 1048576;
    rt->bytes_read = 0;
    rt->has_audio = 0;
    rt->has_video = 0;
    rt->received_metadata = 0;
    rt->last_bytes_read = 0;
    rt->server_bw = 2500000;
    rt->duration = 0;
    av_log(s, AV_LOG_DEBUG, "Proto = %s, path = %s, app = %s, fname = %s\n",
           proto, path, rt->app, rt->playpath);
    if (!rt->listen) {
        if ((ret = gen_connect(s, rt)) < 0)
            goto fail;
    } else {
        if ((ret = read_connect(s, s->priv_data)) < 0)
            goto fail;
    }
    do {
        ret = get_packet(s, 1);
    } while (ret == AVERROR(EAGAIN));
    if (ret < 0)
        goto fail;
    if (rt->do_reconnect) {
        int i;
        ffurl_close(rt->stream);
        rt->stream       = NULL;
        rt->do_reconnect = 0;
        rt->nb_invokes   = 0;
        for (i = 0; i < 2; i++)
            memset(rt->prev_pkt[i], 0,
                   sizeof(**rt->prev_pkt) * rt->nb_prev_pkt[i]);
        free_tracked_methods(rt);
        goto reconnect;
    }
    if (rt->is_input) {
        
        rt->flv_size = 13;
        if ((ret = av_reallocp(&rt->flv_data, rt->flv_size)) < 0)
            goto fail;
        rt->flv_off  = 0;
        memcpy(rt->flv_data, "FLV\1\0\0\0\0\011\0\0\0\0", rt->flv_size);
        
        
        
        
        
        while (!rt->has_audio && !rt->has_video && !rt->received_metadata) {
            if ((ret = get_packet(s, 0)) < 0)
               goto fail;
        }
        
        
        
        if (rt->has_audio) {
            rt->flv_data[4] |= FLV_HEADER_FLAG_HASAUDIO;
        }
        if (rt->has_video) {
            rt->flv_data[4] |= FLV_HEADER_FLAG_HASVIDEO;
        }
        
        
        
        if (!rt->received_metadata && rt->duration > 0) {
            if ((ret = inject_fake_duration_metadata(rt)) < 0)
                goto fail;
        }
    } else {
        rt->flv_size = 0;
        rt->flv_data = NULL;
        rt->flv_off  = 0;
        rt->skip_bytes = 13;
    }
    s->max_packet_size = rt->stream->max_packet_size;
    s->is_streamed     = 1;
    return 0;
fail:
    av_dict_free(&opts);
    rtmp_close(s);
    return ret;
}

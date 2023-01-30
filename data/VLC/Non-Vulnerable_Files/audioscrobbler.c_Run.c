 *****************************************************************************/
static void *Run(void *data)
{
    intf_thread_t          *p_intf = data;
    uint8_t                 p_buffer[1024];
    int                     canc = vlc_savecancel();
    bool                    b_handshaked = false;
    /* data about audioscrobbler session */
    mtime_t                 next_exchange = -1; /**< when can we send data  */
    unsigned int            i_interval = 0;     /**< waiting interval (secs)*/
    intf_sys_t *p_sys = p_intf->p_sys;
    /* main loop */
    for (;;)
    {
        vlc_restorecancel(canc);
        vlc_mutex_lock(&p_sys->lock);
        mutex_cleanup_push(&p_sys->lock);
        do
            vlc_cond_wait(&p_sys->wait, &p_sys->lock);
        while (mdate() < next_exchange);
        vlc_cleanup_run();
        canc = vlc_savecancel();
        /* handshake if needed */
        if (!b_handshaked)
        {
            msg_Dbg(p_intf, "Handshaking with last.fm ...");
            switch(Handshake(p_intf))
            {
                case VLC_ENOMEM:
                    goto out;
                case VLC_ENOVAR:
                    /* username not set */
                    dialog_Fatal(p_intf,
                        _("Last.fm username not set"),
                        "%s", _("Please set a username or disable the "
                        "audioscrobbler plugin, and restart VLC.\n"
                        "Visit http://www.last.fm/join/ to get an account."));
                    goto out;
                case VLC_SUCCESS:
                    msg_Dbg(p_intf, "Handshake successful :)");
                    b_handshaked = true;
                    i_interval = 0;
                    next_exchange = mdate();
                    break;
                case VLC_AUDIOSCROBBLER_EFATAL:
                    msg_Warn(p_intf, "Exiting...");
                    goto out;
                case VLC_EGENERIC:
                default:
                    /* protocol error : we'll try later */
                    HandleInterval(&next_exchange, &i_interval);
                    break;
            }
            /* if handshake failed let's restart the loop */
            if (!b_handshaked)
                continue;
        }
        msg_Dbg(p_intf, "Going to submit some data...");
        char *psz_submit;
        if (asprintf(&psz_submit, "s=%s", p_sys->psz_auth_token) == -1)
            break;
        /* forge the HTTP POST request */
        vlc_mutex_lock(&p_sys->lock);
        audioscrobbler_song_t *p_song;
        for (int i_song = 0 ; i_song < p_sys->i_songs ; i_song++)
        {
            char *psz_submit_song, *psz_submit_tmp;
            p_song = &p_sys->p_queue[i_song];
            if (asprintf(&psz_submit_song,
                    "&a%%5B%d%%5D=%s"
                    "&t%%5B%d%%5D=%s"
                    "&i%%5B%d%%5D=%u"
                    "&o%%5B%d%%5D=P"
                    "&r%%5B%d%%5D="
                    "&l%%5B%d%%5D=%d"
                    "&b%%5B%d%%5D=%s"
                    "&n%%5B%d%%5D=%s"
                    "&m%%5B%d%%5D=%s",
                    i_song, p_song->psz_a,
                    i_song, p_song->psz_t,
                    i_song, (unsigned)p_song->date, /* HACK: %ju (uintmax_t) unsupported on Windows */
                    i_song,
                    i_song,
                    i_song, p_song->i_l,
                    i_song, p_song->psz_b,
                    i_song, p_song->psz_n,
                    i_song, p_song->psz_m
           ) == -1)
            {   /* Out of memory */
                vlc_mutex_unlock(&p_sys->lock);
                goto out;
            }
            psz_submit_tmp = psz_submit;
            if (asprintf(&psz_submit, "%s%s",
                    psz_submit_tmp, psz_submit_song) == -1)
            {   /* Out of memory */
                free(psz_submit_tmp);
                free(psz_submit_song);
                vlc_mutex_unlock(&p_sys->lock);
                goto out;
            }
            free(psz_submit_song);
            free(psz_submit_tmp);
        }
        vlc_mutex_unlock(&p_sys->lock);
        int i_post_socket = net_ConnectTCP(p_intf, p_sys->p_submit_url.psz_host,
                                        p_sys->p_submit_url.i_port);
        if (i_post_socket == -1)
        {
            /* If connection fails, we assume we must handshake again */
            HandleInterval(&next_exchange, &i_interval);
            b_handshaked = false;
            free(psz_submit);
            continue;
        }
        /* we transmit the data */
        int i_net_ret = net_Printf(p_intf, i_post_socket, NULL,
            "POST %s HTTP/1.1\n"
            "Accept-Encoding: identity\n"
            "Content-length: %zu\n"
            "Connection: close\n"
            "Content-type: application/x-www-form-urlencoded\n"
            "Host: %s\n"
            "User-agent: VLC media player/"VERSION"\r\n"
            "\r\n"
            "%s\r\n"
            "\r\n",
            p_sys->p_submit_url.psz_path, strlen(psz_submit),
            p_sys->p_submit_url.psz_host, psz_submit
       );
        free(psz_submit);
        if (i_net_ret == -1)
        {
            /* If connection fails, we assume we must handshake again */
            HandleInterval(&next_exchange, &i_interval);
            b_handshaked = false;
            continue;
        }
        i_net_ret = net_Read(p_intf, i_post_socket, NULL,
                    p_buffer, sizeof(p_buffer) - 1, false);
        if (i_net_ret <= 0)
        {
            /* if we get no answer, something went wrong : try again */
            continue;
        }
        net_Close(i_post_socket);
        p_buffer[i_net_ret] = '\0';
        char *failed = strstr((char *) p_buffer, "FAILED");
        if (failed)
        {
            msg_Warn(p_intf, "%s", failed);
            HandleInterval(&next_exchange, &i_interval);
            continue;
        }
        if (strstr((char *) p_buffer, "BADSESSION"))
        {
            msg_Err(p_intf, "Authentication failed (BADSESSION), are you connected to last.fm with another program ?");
            b_handshaked = false;
            HandleInterval(&next_exchange, &i_interval);
            continue;
        }
        if (strstr((char *) p_buffer, "OK"))
        {
            for (int i = 0; i < p_sys->i_songs; i++)
                DeleteSong(&p_sys->p_queue[i]);
            p_sys->i_songs = 0;
            i_interval = 0;
            next_exchange = mdate();
            msg_Dbg(p_intf, "Submission successful!");
        }
        else
        {
            msg_Err(p_intf, "Authentication failed, handshaking again (%s)",
                             p_buffer);
            b_handshaked = false;
            HandleInterval(&next_exchange, &i_interval);
        }
    }
out:
    vlc_restorecancel(canc);
    return NULL;
}

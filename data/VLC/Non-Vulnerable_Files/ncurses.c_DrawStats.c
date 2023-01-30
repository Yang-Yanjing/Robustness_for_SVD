}
static int DrawStats(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    input_thread_t *p_input = sys->p_input;
    input_item_t *item;
    input_stats_t *p_stats;
    int l = 0, i_audio = 0, i_video = 0;
    if (!p_input)
        return 0;
    item = input_GetItem(p_input);
    assert(item);
    vlc_mutex_lock(&item->lock);
    p_stats = item->p_stats;
    vlc_mutex_lock(&p_stats->lock);
    for (int i = 0; i < item->i_es ; i++) {
        i_audio += (item->es[i]->i_cat == AUDIO_ES);
        i_video += (item->es[i]->i_cat == VIDEO_ES);
    }
    /* Input */
    if (sys->color) color_set(C_CATEGORY, NULL);
    MainBoxWrite(sys, l++, _("+-[Incoming]"));
    if (sys->color) color_set(C_DEFAULT, NULL);
    MainBoxWrite(sys, l++, _("| input bytes read : %8.0f KiB"),
            (float)(p_stats->i_read_bytes)/1024);
    MainBoxWrite(sys, l++, _("| input bitrate    :   %6.0f kb/s"),
            p_stats->f_input_bitrate*8000);
    MainBoxWrite(sys, l++, _("| demux bytes read : %8.0f KiB"),
            (float)(p_stats->i_demux_read_bytes)/1024);
    MainBoxWrite(sys, l++, _("| demux bitrate    :   %6.0f kb/s"),
            p_stats->f_demux_bitrate*8000);
    /* Video */
    if (i_video) {
        if (sys->color) color_set(C_CATEGORY, NULL);
        MainBoxWrite(sys, l++, _("+-[Video Decoding]"));
        if (sys->color) color_set(C_DEFAULT, NULL);
        MainBoxWrite(sys, l++, _("| video decoded    :    %5"PRIi64),
                p_stats->i_decoded_video);
        MainBoxWrite(sys, l++, _("| frames displayed :    %5"PRIi64),
                p_stats->i_displayed_pictures);
        MainBoxWrite(sys, l++, _("| frames lost      :    %5"PRIi64),
                p_stats->i_lost_pictures);
    }
    /* Audio*/
    if (i_audio) {
        if (sys->color) color_set(C_CATEGORY, NULL);
        MainBoxWrite(sys, l++, _("+-[Audio Decoding]"));
        if (sys->color) color_set(C_DEFAULT, NULL);
        MainBoxWrite(sys, l++, _("| audio decoded    :    %5"PRIi64),
                p_stats->i_decoded_audio);
        MainBoxWrite(sys, l++, _("| buffers played   :    %5"PRIi64),
                p_stats->i_played_abuffers);
        MainBoxWrite(sys, l++, _("| buffers lost     :    %5"PRIi64),
                p_stats->i_lost_abuffers);
    }
    /* Sout */
    if (sys->color) color_set(C_CATEGORY, NULL);
    MainBoxWrite(sys, l++, _("+-[Streaming]"));
    if (sys->color) color_set(C_DEFAULT, NULL);
    MainBoxWrite(sys, l++, _("| packets sent     :    %5"PRIi64), p_stats->i_sent_packets);
    MainBoxWrite(sys, l++, _("| bytes sent       : %8.0f KiB"),
            (float)(p_stats->i_sent_bytes)/1025);
    MainBoxWrite(sys, l++, _("| sending bitrate  :   %6.0f kb/s"),
            p_stats->f_send_bitrate*8000);
    if (sys->color) color_set(C_DEFAULT, NULL);
    vlc_mutex_unlock(&p_stats->lock);
    vlc_mutex_unlock(&item->lock);
    return l;
}

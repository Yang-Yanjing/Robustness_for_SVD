}
static int updateStatistics( intf_thread_t *p_intf, input_item_t *p_item )
{
    if( !p_item ) return VLC_EGENERIC;
    vlc_mutex_lock( &p_item->lock );
    vlc_mutex_lock( &p_item->p_stats->lock );
    msg_rc( "+----[ begin of statistical info ]" );
    /* Input */
    msg_rc("%s", _("+-[Incoming]"));
    msg_rc(_("| input bytes read : %8.0f KiB"),
            (float)(p_item->p_stats->i_read_bytes)/1024 );
    msg_rc(_("| input bitrate    :   %6.0f kb/s"),
            (float)(p_item->p_stats->f_input_bitrate)*8000 );
    msg_rc(_("| demux bytes read : %8.0f KiB"),
            (float)(p_item->p_stats->i_demux_read_bytes)/1024 );
    msg_rc(_("| demux bitrate    :   %6.0f kb/s"),
            (float)(p_item->p_stats->f_demux_bitrate)*8000 );
    msg_rc(_("| demux corrupted  :    %5"PRIi64),
            p_item->p_stats->i_demux_corrupted );
    msg_rc(_("| discontinuities  :    %5"PRIi64),
            p_item->p_stats->i_demux_discontinuity );
    msg_rc("|");
    /* Video */
    msg_rc("%s", _("+-[Video Decoding]"));
    msg_rc(_("| video decoded    :    %5"PRIi64),
            p_item->p_stats->i_decoded_video );
    msg_rc(_("| frames displayed :    %5"PRIi64),
            p_item->p_stats->i_displayed_pictures );
    msg_rc(_("| frames lost      :    %5"PRIi64),
            p_item->p_stats->i_lost_pictures );
    msg_rc("|");
    /* Audio*/
    msg_rc("%s", _("+-[Audio Decoding]"));
    msg_rc(_("| audio decoded    :    %5"PRIi64),
            p_item->p_stats->i_decoded_audio );
    msg_rc(_("| buffers played   :    %5"PRIi64),
            p_item->p_stats->i_played_abuffers );
    msg_rc(_("| buffers lost     :    %5"PRIi64),
            p_item->p_stats->i_lost_abuffers );
    msg_rc("|");
    /* Sout */
    msg_rc("%s", _("+-[Streaming]"));
    msg_rc(_("| packets sent     :    %5"PRIi64),
           p_item->p_stats->i_sent_packets );
    msg_rc(_("| bytes sent       : %8.0f KiB"),
            (float)(p_item->p_stats->i_sent_bytes)/1024 );
    msg_rc(_("| sending bitrate  :   %6.0f kb/s"),
            (float)(p_item->p_stats->f_send_bitrate*8)*1000 );
    msg_rc("|");
    msg_rc( "+----[ end of statistical info ]" );
    vlc_mutex_unlock( &p_item->p_stats->lock );
    vlc_mutex_unlock( &p_item->lock );
    return VLC_SUCCESS;
}

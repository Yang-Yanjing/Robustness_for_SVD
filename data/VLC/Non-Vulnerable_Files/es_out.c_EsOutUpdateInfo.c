 ****************************************************************************/
static void EsOutUpdateInfo( es_out_t *out, es_out_id_t *es, const es_format_t *fmt, const vlc_meta_t *p_meta )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    const es_format_t *p_fmt_es = &es->fmt;
    lldiv_t         div;
    if( es->fmt.i_cat == fmt->i_cat )
    {
        es_format_t update = *fmt;
        update.i_id = es->i_meta_id;
        update.i_codec = es->fmt.i_codec;
        update.i_original_fourcc = es->fmt.i_original_fourcc;
        input_item_UpdateTracksInfo(input_GetItem(p_input), &update);
    }
    /* Create category */
    char psz_cat[128];
    snprintf( psz_cat, sizeof(psz_cat),_("Stream %d"), es->i_meta_id );
    info_category_t *p_cat = info_category_New( psz_cat );
    if( !p_cat )
        return;
    /* Add information */
    const char *psz_type;
    switch( fmt->i_cat )
    {
    case AUDIO_ES:
        psz_type = _("Audio");
        break;
    case VIDEO_ES:
        psz_type = _("Video");
        break;
    case SPU_ES:
        psz_type = _("Subtitle");
        break;
    default:
        psz_type = NULL;
        break;
    }
    if( psz_type )
        info_category_AddInfo( p_cat, _("Type"), "%s", psz_type );
    if( es->i_meta_id != es->i_id )
        info_category_AddInfo( p_cat, _("Original ID"),
                       "%d", es->i_id );
    const char *psz_codec_description =
        vlc_fourcc_GetDescription( p_fmt_es->i_cat, p_fmt_es->i_codec );
    const vlc_fourcc_t i_codec_fourcc = ( p_fmt_es->i_original_fourcc )?
                               p_fmt_es->i_original_fourcc : p_fmt_es->i_codec;
    if( psz_codec_description && *psz_codec_description )
        info_category_AddInfo( p_cat, _("Codec"), "%s (%.4s)",
                               psz_codec_description, (char*)&i_codec_fourcc );
    else if ( i_codec_fourcc != VLC_FOURCC(0,0,0,0) )
        info_category_AddInfo( p_cat, _("Codec"), "%.4s",
                               (char*)&i_codec_fourcc );
    if( es->psz_language && *es->psz_language )
        info_category_AddInfo( p_cat, _("Language"), "%s",
                               es->psz_language );
    if( fmt->psz_description && *fmt->psz_description )
        info_category_AddInfo( p_cat, _("Description"), "%s",
                               fmt->psz_description );
    switch( fmt->i_cat )
    {
    case AUDIO_ES:
        info_category_AddInfo( p_cat, _("Type"), _("Audio") );
        if( fmt->audio.i_physical_channels )
            info_category_AddInfo( p_cat, _("Channels"), "%s",
                                   _( aout_FormatPrintChannels( &fmt->audio ) ) );
        if( fmt->audio.i_rate > 0 )
        {
            info_category_AddInfo( p_cat, _("Sample rate"), _("%u Hz"),
                                   fmt->audio.i_rate );
            /* FIXME that should be removed or improved ! (used by text/strings.c) */
            var_SetInteger( p_input, "sample-rate", fmt->audio.i_rate );
        }
        unsigned int i_bitspersample = fmt->audio.i_bitspersample;
        if( i_bitspersample <= 0 )
            i_bitspersample = aout_BitsPerSample( p_fmt_es->i_codec );
        if( i_bitspersample > 0 )
            info_category_AddInfo( p_cat, _("Bits per sample"), "%u",
                                   i_bitspersample );
        if( fmt->i_bitrate > 0 )
        {
            info_category_AddInfo( p_cat, _("Bitrate"), _("%u kb/s"),
                                   fmt->i_bitrate / 1000 );
            /* FIXME that should be removed or improved ! (used by text/strings.c) */
            var_SetInteger( p_input, "bit-rate", fmt->i_bitrate );
        }
        for( int i = 0; i < AUDIO_REPLAY_GAIN_MAX; i++ )
        {
            const audio_replay_gain_t *p_rg = &fmt->audio_replay_gain;
            if( !p_rg->pb_gain[i] )
                continue;
            const char *psz_name;
            if( i == AUDIO_REPLAY_GAIN_TRACK )
                psz_name = _("Track replay gain");
            else
                psz_name = _("Album replay gain");
            info_category_AddInfo( p_cat, psz_name, _("%.2f dB"),
                                   p_rg->pf_gain[i] );
        }
        break;
    case VIDEO_ES:
        info_category_AddInfo( p_cat, _("Type"), _("Video") );
        if( fmt->video.i_width > 0 && fmt->video.i_height > 0 )
            info_category_AddInfo( p_cat, _("Resolution"), "%ux%u",
                                   fmt->video.i_width, fmt->video.i_height );
        if( fmt->video.i_visible_width > 0 &&
            fmt->video.i_visible_height > 0 )
            info_category_AddInfo( p_cat, _("Display resolution"), "%ux%u",
                                   fmt->video.i_visible_width,
                                   fmt->video.i_visible_height);
       if( fmt->video.i_frame_rate > 0 &&
           fmt->video.i_frame_rate_base > 0 )
       {
           div = lldiv( (float)fmt->video.i_frame_rate /
                               fmt->video.i_frame_rate_base * 1000000,
                               1000000 );
           if( div.rem > 0 )
               info_category_AddInfo( p_cat, _("Frame rate"), "%"PRId64".%06u",
                                      div.quot, (unsigned int )div.rem );
           else
               info_category_AddInfo( p_cat, _("Frame rate"), "%"PRId64,
                                      div.quot );
       }
       if( fmt->i_codec != p_fmt_es->i_codec )
       {
           const char *psz_chroma_description =
                vlc_fourcc_GetDescription( VIDEO_ES, fmt->i_codec );
           if( psz_chroma_description )
               info_category_AddInfo( p_cat, _("Decoded format"), "%s",
                                      psz_chroma_description );
       }
       break;
    case SPU_ES:
        info_category_AddInfo( p_cat, _("Type"), _("Subtitle") );
        break;
    default:
        break;
    }
    /* Append generic meta */
    if( p_meta )
    {
        char **ppsz_all_keys = vlc_meta_CopyExtraNames( p_meta );
        for( int i = 0; ppsz_all_keys && ppsz_all_keys[i]; i++ )
        {
            char *psz_key = ppsz_all_keys[i];
            const char *psz_value = vlc_meta_GetExtra( p_meta, psz_key );
            if( psz_value )
                info_category_AddInfo( p_cat, vlc_gettext(psz_key), "%s",
                                       vlc_gettext(psz_value) );
            free( psz_key );
        }
        free( ppsz_all_keys );
    }
    /* */
    input_Control( p_input, INPUT_REPLACE_INFOS, p_cat );
}

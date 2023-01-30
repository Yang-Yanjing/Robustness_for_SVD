}
void es_format_Init( es_format_t *fmt,
                     int i_cat, vlc_fourcc_t i_codec )
{
    fmt->i_cat                  = i_cat;
    fmt->i_codec                = i_codec;
    fmt->i_original_fourcc      = 0;
    fmt->i_profile              = -1;
    fmt->i_level                = -1;
    fmt->i_id                   = -1;
    fmt->i_group                = 0;
    fmt->i_priority             = ES_PRIORITY_SELECTABLE_MIN;
    fmt->psz_language           = NULL;
    fmt->psz_description        = NULL;
    fmt->i_extra_languages      = 0;
    fmt->p_extra_languages      = NULL;
    memset( &fmt->audio, 0, sizeof(audio_format_t) );
    memset( &fmt->audio_replay_gain, 0, sizeof(audio_replay_gain_t) );
    memset( &fmt->video, 0, sizeof(video_format_t) );
    memset( &fmt->subs, 0, sizeof(subs_format_t) );
    fmt->b_packetized           = true;
    fmt->i_bitrate              = 0;
    fmt->i_extra                = 0;
    fmt->p_extra                = NULL;
}

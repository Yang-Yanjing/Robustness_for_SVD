int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    AVCodecContext *p_context;
    AVCodec *p_codec = NULL;
    unsigned i_codec_id;
    int i_cat;
    const char *psz_namecodec;
    float f_val;
    char *psz_val;
    /* Initialization must be done before avcodec_find_encoder() */
    vlc_init_avcodec(p_this);
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    if( p_enc->fmt_out.i_codec == VLC_CODEC_MP1V )
    {
        i_cat = VIDEO_ES;
        i_codec_id = AV_CODEC_ID_MPEG1VIDEO;
        psz_namecodec = "MPEG-1 video";
    }
    else if( !GetFfmpegCodec( p_enc->fmt_out.i_codec, &i_cat, &i_codec_id,
                             &psz_namecodec ) )
    {
        if( FindFfmpegChroma( p_enc->fmt_out.i_codec ) == PIX_FMT_NONE )
            return VLC_EGENERIC; /* handed chroma output */
        i_cat      = VIDEO_ES;
        i_codec_id = AV_CODEC_ID_RAWVIDEO;
        psz_namecodec = "Raw video";
    }
    if( i_cat == UNKNOWN_ES )
        return VLC_EGENERIC;
    if( p_enc->fmt_out.i_cat == VIDEO_ES && i_cat != VIDEO_ES )
    {
        msg_Err( p_enc, "\"%s\" is not a video encoder", psz_namecodec );
        dialog_Fatal( p_enc, _("Streaming / Transcoding failed"),
                        _("\"%s\" is no video encoder."), psz_namecodec );
        return VLC_EGENERIC;
    }
    if( p_enc->fmt_out.i_cat == AUDIO_ES && i_cat != AUDIO_ES )
    {
        msg_Err( p_enc, "\"%s\" is not an audio encoder", psz_namecodec );
        dialog_Fatal( p_enc, _("Streaming / Transcoding failed"),
                        _("\"%s\" is no audio encoder."), psz_namecodec );
        return VLC_EGENERIC;
    }
    if( p_enc->fmt_out.i_cat == SPU_ES )
    {
        /* We don't support subtitle encoding */
        return VLC_EGENERIC;
    }
    char *psz_encoder = var_GetString( p_this, ENC_CFG_PREFIX "codec" );
    if( psz_encoder && *psz_encoder )
    {
        p_codec = avcodec_find_encoder_by_name( psz_encoder );
        if( !p_codec )
            msg_Err( p_this, "Encoder `%s' not found", psz_encoder );
        else if( p_codec->id != i_codec_id )
        {
            msg_Err( p_this, "Encoder `%s' can't handle %4.4s",
                    psz_encoder, (char*)&p_enc->fmt_out.i_codec );
            p_codec = NULL;
        }
    }
    free( psz_encoder );
    if( !p_codec )
        p_codec = avcodec_find_encoder( i_codec_id );
    if( !p_codec )
    {
        msg_Err( p_enc, "cannot find encoder %s\n"
"*** Your Libav/FFmpeg installation is crippled.   ***\n"
"*** Please check with your Libav/FFmpeg packager. ***\n"
"*** This is NOT a VLC media player issue.   ***", psz_namecodec );
        dialog_Fatal( p_enc, _("Streaming / Transcoding failed"), _(
/* I have had enough of all these MPEG-3 transcoding bug reports.
 * Downstream packager, you had better not patch this out, or I will be really
 * annoyed. Think about it - you don't want to fork the VLC translation files,
 * do you? -- Courmisch, 2008-10-22 */
"It seems your Libav/FFmpeg (libavcodec) installation lacks the following encoder:\n"
"%s.\n"
"If you don't know how to fix this, ask for support from your distribution.\n"
"\n"
"This is not an error inside VLC media player.\n"
"Do not contact the VideoLAN project about this issue.\n"),
            psz_namecodec );
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the encoder's structure */
    if( ( p_sys = calloc( 1, sizeof(encoder_sys_t) ) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_sys->i_samples_delay = 0;
    p_sys->p_codec = p_codec;
    p_sys->b_planar = false;
    p_sys->p_buffer = NULL;
    p_sys->p_interleave_buf = NULL;
    p_sys->i_buffer_out = 0;
    p_context = avcodec_alloc_context3(p_codec);
    p_sys->p_context = p_context;
    p_sys->p_context->codec_id = p_sys->p_codec->id;
    p_context->thread_type = 0;
    p_context->debug = var_InheritInteger( p_enc, "avcodec-debug" );
    p_context->opaque = (void *)p_this;
    p_sys->i_key_int = var_GetInteger( p_enc, ENC_CFG_PREFIX "keyint" );
    p_sys->i_b_frames = var_GetInteger( p_enc, ENC_CFG_PREFIX "bframes" );
    p_sys->i_vtolerance = var_GetInteger( p_enc, ENC_CFG_PREFIX "vt" ) * 1000;
    p_sys->b_interlace = var_GetBool( p_enc, ENC_CFG_PREFIX "interlace" );
    p_sys->b_interlace_me = var_GetBool( p_enc, ENC_CFG_PREFIX "interlace-me" );
    p_sys->b_pre_me = var_GetBool( p_enc, ENC_CFG_PREFIX "pre-me" );
    p_sys->b_hurry_up = var_GetBool( p_enc, ENC_CFG_PREFIX "hurry-up" );
    if( p_sys->b_hurry_up )
    {
        /* hurry up mode needs noise reduction, even small */
        p_sys->i_noise_reduction = 1;
    }
    p_sys->i_rc_buffer_size = var_GetInteger( p_enc, ENC_CFG_PREFIX "rc-buffer-size" );
    p_sys->f_rc_buffer_aggressivity = var_GetFloat( p_enc, ENC_CFG_PREFIX "rc-buffer-aggressivity" );
    p_sys->f_i_quant_factor = var_GetFloat( p_enc, ENC_CFG_PREFIX "i-quant-factor" );
    p_sys->i_noise_reduction = var_GetInteger( p_enc, ENC_CFG_PREFIX "noise-reduction" );
    p_sys->b_mpeg4_matrix = var_GetBool( p_enc, ENC_CFG_PREFIX "mpeg4-matrix" );
    f_val = var_GetFloat( p_enc, ENC_CFG_PREFIX "qscale" );
    p_sys->i_quality = 0;
    if( f_val < 0.01 || f_val > 255.0 )
        f_val = 0;
    else
        p_sys->i_quality = (int)(FF_QP2LAMBDA * f_val + 0.5);
    psz_val = var_GetString( p_enc, ENC_CFG_PREFIX "hq" );
    p_sys->i_hq = FF_MB_DECISION_RD;
    if( psz_val && *psz_val )
    {
        if( !strcmp( psz_val, "rd" ) )
            p_sys->i_hq = FF_MB_DECISION_RD;
        else if( !strcmp( psz_val, "bits" ) )
            p_sys->i_hq = FF_MB_DECISION_BITS;
        else if( !strcmp( psz_val, "simple" ) )
            p_sys->i_hq = FF_MB_DECISION_SIMPLE;
        else
            p_sys->i_hq = FF_MB_DECISION_RD;
    }
    else
        p_sys->i_hq = FF_MB_DECISION_RD;
    free( psz_val );
    p_sys->i_qmin = var_GetInteger( p_enc, ENC_CFG_PREFIX "qmin" );
    p_sys->i_qmax = var_GetInteger( p_enc, ENC_CFG_PREFIX "qmax" );
    p_sys->b_trellis = var_GetBool( p_enc, ENC_CFG_PREFIX "trellis" );
    p_context->strict_std_compliance = var_GetInteger( p_enc, ENC_CFG_PREFIX "strict" );
    p_sys->f_lumi_masking = var_GetFloat( p_enc, ENC_CFG_PREFIX "lumi-masking" );
    p_sys->f_dark_masking = var_GetFloat( p_enc, ENC_CFG_PREFIX "dark-masking" );
    p_sys->f_p_masking = var_GetFloat( p_enc, ENC_CFG_PREFIX "p-masking" );
    p_sys->f_border_masking = var_GetFloat( p_enc, ENC_CFG_PREFIX "border-masking" );
#if (LIBAVCODEC_VERSION_MAJOR < 55)
    p_sys->i_luma_elim = var_GetInteger( p_enc, ENC_CFG_PREFIX "luma-elim-threshold" );
    p_sys->i_chroma_elim = var_GetInteger( p_enc, ENC_CFG_PREFIX "chroma-elim-threshold" );
#endif
    psz_val = var_GetString( p_enc, ENC_CFG_PREFIX "aac-profile" );
    /* libavcodec uses faac encoder atm, and it has issues with
     * other than low-complexity profile, so default to that */
    p_sys->i_aac_profile = FF_PROFILE_AAC_LOW;
    if( psz_val && *psz_val )
    {
        if( !strncmp( psz_val, "main", 4 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_MAIN;
        else if( !strncmp( psz_val, "low", 3 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_LOW;
        else if( !strncmp( psz_val, "ssr", 3 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_SSR;
        else if( !strncmp( psz_val, "ltp", 3 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_LTP;
#if LIBAVCODEC_VERSION_CHECK( 54, 19, 0, 35, 100 )
/* These require libavcodec with libfdk-aac */
        else if( !strncmp( psz_val, "hev2", 4 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_HE_V2;
        else if( !strncmp( psz_val, "hev1", 4 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_HE;
        else if( !strncmp( psz_val, "ld", 2 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_LD;
        else if( !strncmp( psz_val, "eld", 3 ) )
            p_sys->i_aac_profile = FF_PROFILE_AAC_ELD;
#endif
        else
        {
            msg_Warn( p_enc, "unknown AAC profile requested, setting it to low" );
            p_sys->i_aac_profile = FF_PROFILE_AAC_LOW;
        }
    }
    free( psz_val );
    if( p_enc->fmt_in.i_cat == VIDEO_ES )
    {
        if( !p_enc->fmt_in.video.i_visible_width || !p_enc->fmt_in.video.i_visible_height )
        {
            msg_Warn( p_enc, "invalid size %ix%i", p_enc->fmt_in.video.i_visible_width,
                      p_enc->fmt_in.video.i_visible_height );
            free( p_sys );
            return VLC_EGENERIC;
        }
        p_context->codec_type = AVMEDIA_TYPE_VIDEO;
        p_context->width = p_enc->fmt_in.video.i_visible_width;
        p_context->height = p_enc->fmt_in.video.i_visible_height;
        probe_video_frame_rate( p_enc, p_context, p_codec );
        /* Defaults from ffmpeg.c */
        p_context->qblur = 0.5;
        p_context->qcompress = 0.5;
        p_context->b_quant_offset = 1.25;
        p_context->b_quant_factor = 1.25;
        p_context->i_quant_offset = 0.0;
        p_context->i_quant_factor = -0.8;
        p_context->lumi_masking = p_sys->f_lumi_masking;
        p_context->dark_masking = p_sys->f_dark_masking;
        p_context->p_masking = p_sys->f_p_masking;
        p_context->border_masking = p_sys->f_border_masking;
#if (LIBAVCODEC_VERSION_MAJOR < 55)
        p_context->luma_elim_threshold = p_sys->i_luma_elim;
        p_context->chroma_elim_threshold = p_sys->i_chroma_elim;
#endif
        if( p_sys->i_key_int > 0 )
            p_context->gop_size = p_sys->i_key_int;
        p_context->max_b_frames =
            VLC_CLIP( p_sys->i_b_frames, 0, FF_MAX_B_FRAMES );
        p_context->b_frame_strategy = 0;
        if( !p_context->max_b_frames  &&
            (  p_enc->fmt_out.i_codec == VLC_CODEC_MPGV ||
               p_enc->fmt_out.i_codec == VLC_CODEC_MP2V ) )
            p_context->flags |= CODEC_FLAG_LOW_DELAY;
        av_reduce( &p_context->sample_aspect_ratio.num,
                   &p_context->sample_aspect_ratio.den,
                   p_enc->fmt_in.video.i_sar_num,
                   p_enc->fmt_in.video.i_sar_den, 1 << 30 );
        p_enc->fmt_in.i_codec = VLC_CODEC_I420;
        /* Very few application support YUV in TIFF, not even VLC */
        if( p_enc->fmt_out.i_codec == VLC_CODEC_TIFF )
            p_enc->fmt_in.i_codec = VLC_CODEC_RGB24;
        p_enc->fmt_in.video.i_chroma = p_enc->fmt_in.i_codec;
        GetFfmpegChroma( &p_context->pix_fmt, &p_enc->fmt_in.video );
        if( p_codec->pix_fmts )
        {
            const enum PixelFormat *p = p_codec->pix_fmts;
            for( ; *p != -1; p++ )
            {
                if( *p == p_context->pix_fmt ) break;
            }
            if( *p == -1 ) p_context->pix_fmt = p_codec->pix_fmts[0];
            GetVlcChroma( &p_enc->fmt_in.video, p_context->pix_fmt );
            p_enc->fmt_in.i_codec = p_enc->fmt_in.video.i_chroma;
        }
        if ( p_sys->f_i_quant_factor != 0.0 )
            p_context->i_quant_factor = p_sys->f_i_quant_factor;
        p_context->noise_reduction = p_sys->i_noise_reduction;
        if ( p_sys->b_mpeg4_matrix )
        {
            p_context->intra_matrix = mpeg4_default_intra_matrix;
            p_context->inter_matrix = mpeg4_default_non_intra_matrix;
        }
        if ( p_sys->b_pre_me )
        {
            p_context->pre_me = 1;
            p_context->me_pre_cmp = FF_CMP_CHROMA;
        }
        if ( p_sys->b_interlace )
        {
            if ( p_context->height <= 280 )
            {
                if ( p_context->height != 16 || p_context->width != 16 )
                    msg_Warn( p_enc,
                        "disabling interlaced video because height=%d <= 280",
                        p_context->height );
            }
            else
            {
                p_context->flags |= CODEC_FLAG_INTERLACED_DCT;
                if ( p_sys->b_interlace_me )
                    p_context->flags |= CODEC_FLAG_INTERLACED_ME;
            }
        }
        p_context->trellis = p_sys->b_trellis;
        if ( p_sys->i_qmin > 0 && p_sys->i_qmin == p_sys->i_qmax )
            p_context->flags |= CODEC_FLAG_QSCALE;
        /* These codecs cause libavcodec to exit if thread_count is > 1.
           See libavcodec/mpegvideo_enc.c:MPV_encode_init and
           libavcodec/svq3.c , WMV2 calls MPV_encode_init also.
         */
        if ( i_codec_id == AV_CODEC_ID_FLV1 ||
             i_codec_id == AV_CODEC_ID_H261 ||
             i_codec_id == AV_CODEC_ID_LJPEG ||
             i_codec_id == AV_CODEC_ID_MJPEG ||
             i_codec_id == AV_CODEC_ID_H263 ||
             i_codec_id == AV_CODEC_ID_H263P ||
             i_codec_id == AV_CODEC_ID_MSMPEG4V1 ||
             i_codec_id == AV_CODEC_ID_MSMPEG4V2 ||
             i_codec_id == AV_CODEC_ID_MSMPEG4V3 ||
             i_codec_id == AV_CODEC_ID_WMV1 ||
             i_codec_id == AV_CODEC_ID_WMV2 ||
             i_codec_id == AV_CODEC_ID_RV10 ||
             i_codec_id == AV_CODEC_ID_RV20 ||
             i_codec_id == AV_CODEC_ID_SVQ3 )
            p_enc->i_threads = 1;
        if( p_sys->i_vtolerance > 0 )
            p_context->bit_rate_tolerance = p_sys->i_vtolerance;
        /* usually if someone sets bitrate, he likes more to get that bitrate
         * over quality should help 'normal' user to get asked bitrate
         */
        if( p_enc->fmt_out.i_bitrate > 0 && p_sys->i_qmax == 0 && p_sys->i_qmin == 0 )
        {
            p_sys->i_qmax = 51;
            p_sys->i_qmin = 3;
        }
        if( p_sys->i_qmin > 0 )
        {
            p_context->qmin = p_sys->i_qmin;
            p_context->mb_lmin = p_context->lmin = p_sys->i_qmin * FF_QP2LAMBDA;
        }
        if( p_sys->i_qmax > 0 )
        {
            p_context->qmax = p_sys->i_qmax;
            p_context->mb_lmax = p_context->lmax = p_sys->i_qmax * FF_QP2LAMBDA;
        }
        p_context->max_qdiff = 3;
        p_context->mb_decision = p_sys->i_hq;
        if( p_sys->i_quality && !p_enc->fmt_out.i_bitrate )
        {
            p_context->flags |= CODEC_FLAG_QSCALE;
            p_context->global_quality = p_sys->i_quality;
        }
        else
        {
            p_context->rc_qsquish = 1.0;
            /* Default to 1/2 second buffer for given bitrate unless defined otherwise*/
            if( !p_sys->i_rc_buffer_size )
            {
                p_sys->i_rc_buffer_size = p_enc->fmt_out.i_bitrate * 8 / 2;
            }
            msg_Dbg( p_enc, "rc buffer size %d bits", p_sys->i_rc_buffer_size );
            /* Set maxrate/minrate to bitrate to try to get CBR */
            p_context->rc_max_rate = p_enc->fmt_out.i_bitrate;
            p_context->rc_min_rate = p_enc->fmt_out.i_bitrate;
            p_context->rc_buffer_size = p_sys->i_rc_buffer_size;
            /* This is from ffmpeg's ffmpeg.c : */
            p_context->rc_initial_buffer_occupancy
                = p_sys->i_rc_buffer_size * 3/4;
            p_context->rc_buffer_aggressivity = p_sys->f_rc_buffer_aggressivity;
        }
    }
    else if( p_enc->fmt_in.i_cat == AUDIO_ES )
    {
        /* work around bug in libmp3lame encoding */
        if( i_codec_id == AV_CODEC_ID_MP3 && p_enc->fmt_out.audio.i_channels  > 2 )
            p_enc->fmt_out.audio.i_channels = 2;
        p_context->codec_type  = AVMEDIA_TYPE_AUDIO;
        p_context->sample_fmt  = p_codec->sample_fmts ?
                                    p_codec->sample_fmts[0] :
                                    AV_SAMPLE_FMT_S16;
        /* Try to match avcodec input format to vlc format so we could avoid one
           format conversion */
        if( GetVlcAudioFormat( p_context->sample_fmt ) != p_enc->fmt_in.i_codec
            && p_codec->sample_fmts )
        {
            msg_Dbg( p_enc, "Trying to find more suitable sample format instead of %s", av_get_sample_fmt_name( p_context->sample_fmt ) );
            for( unsigned int i=0; p_codec->sample_fmts[i] != -1; i++ )
            {
                if( GetVlcAudioFormat( p_codec->sample_fmts[i] ) == p_enc->fmt_in.i_codec )
                {
                    p_context->sample_fmt = p_codec->sample_fmts[i];
                    msg_Dbg( p_enc, "Using %s as new sample format", av_get_sample_fmt_name( p_context->sample_fmt ) );
                    break;
                }
            }
        }
        p_sys->b_planar = av_sample_fmt_is_planar( p_context->sample_fmt );
        // Try if we can use interleaved format for codec input as VLC doesn't really do planar audio yet
        // FIXME: Remove when planar/interleaved audio in vlc is equally supported
        if( p_sys->b_planar && p_codec->sample_fmts )
        {
            msg_Dbg( p_enc, "Trying to find packet sample format instead of planar %s", av_get_sample_fmt_name( p_context->sample_fmt ) );
            for( unsigned int i=0; p_codec->sample_fmts[i] != -1; i++ )
            {
                if( !av_sample_fmt_is_planar( p_codec->sample_fmts[i] ) )
                {
                    p_context->sample_fmt = p_codec->sample_fmts[i];
                    msg_Dbg( p_enc, "Changing to packet format %s as new sample format", av_get_sample_fmt_name( p_context->sample_fmt ) );
                    break;
                }
            }
        }
        msg_Dbg( p_enc, "Ended up using %s as sample format", av_get_sample_fmt_name( p_context->sample_fmt ) );
        p_enc->fmt_in.i_codec  = GetVlcAudioFormat( p_context->sample_fmt );
        p_sys->b_planar = av_sample_fmt_is_planar( p_context->sample_fmt );
        p_context->sample_rate = p_enc->fmt_out.audio.i_rate;
        date_Init( &p_sys->buffer_date, p_enc->fmt_out.audio.i_rate, 1 );
        date_Set( &p_sys->buffer_date, AV_NOPTS_VALUE );
        p_context->time_base.num = 1;
        p_context->time_base.den = p_context->sample_rate;
        p_context->channels      = p_enc->fmt_out.audio.i_channels;
#if LIBAVUTIL_VERSION_CHECK( 52, 2, 6, 0, 0)
        p_context->channel_layout = channel_mask[p_context->channels][1];
        /* Setup Channel ordering for multichannel audio
         * as VLC channel order isn't same as libavcodec expects
         */
        p_sys->i_channels_to_reorder = 0;
        /* Specified order
         * Copied from audio.c
         */
        const unsigned i_order_max = 8 * sizeof(p_context->channel_layout);
        uint32_t pi_order_dst[AOUT_CHAN_MAX];
        int i_channels_src = 0;
        if( p_context->channel_layout )
        {
            msg_Dbg( p_enc, "Creating channel order for reordering");
            for( unsigned i = 0; i < sizeof(pi_channels_map)/sizeof(*pi_channels_map); i++ )
            {
                if( p_context->channel_layout & pi_channels_map[i][0] )
                {
                    msg_Dbg( p_enc, "%d %"PRIx64" mapped to %"PRIx64"", i_channels_src, pi_channels_map[i][0], pi_channels_map[i][1]);
                    pi_order_dst[i_channels_src++] = pi_channels_map[i][1];
                }
            }
        }
        else
        {
            msg_Dbg( p_enc, "Creating default channel order for reordering");
            /* Create default order  */
            for( unsigned int i = 0; i < __MIN( i_order_max, (unsigned)p_sys->p_context->channels ); i++ )
            {
                if( i < sizeof(pi_channels_map)/sizeof(*pi_channels_map) )
                {
                    msg_Dbg( p_enc, "%d channel is %"PRIx64"", i_channels_src, pi_channels_map[i][1]);
                    pi_order_dst[i_channels_src++] = pi_channels_map[i][1];
                }
            }
        }
        if( i_channels_src != p_context->channels )
            msg_Err( p_enc, "Channel layout not understood" );
        p_sys->i_channels_to_reorder = aout_CheckChannelReorder( NULL, pi_order_dst,
            channel_mask[p_context->channels][0], p_sys->pi_reorder_layout );
#endif
        if ( p_enc->fmt_out.i_codec == VLC_CODEC_MP4A )
        {
            /* XXX: FAAC does resample only when setting the INPUT samplerate
             * to the desired value (-R option of the faac frontend)
            p_enc->fmt_in.audio.i_rate = p_context->sample_rate;*/
            /* vlc should default to low-complexity profile, faac encoder
             * has bug and aac audio has issues otherwise atm */
            p_context->profile = p_sys->i_aac_profile;
        }
    }
    /* Misc parameters */
    p_context->bit_rate = p_enc->fmt_out.i_bitrate;
    /* Set reasonable defaults to VP8, based on
       libvpx-720p preset from libvpx ffmpeg-patch */
    if( i_codec_id == AV_CODEC_ID_VP8 )
    {
        /* Lets give bitrate tolerance */
        p_context->bit_rate_tolerance = __MAX(2 * (int)p_enc->fmt_out.i_bitrate, p_sys->i_vtolerance );
        /* default to 120 frames between keyframe */
        if( !var_GetInteger( p_enc, ENC_CFG_PREFIX "keyint" ) )
            p_context->gop_size = 120;
        /* Don't set rc-values atm, they were from time before
           libvpx was officially in FFmpeg */
        //p_context->rc_max_rate = 24 * 1000 * 1000; //24M
        //p_context->rc_min_rate = 40 * 1000; // 40k
        /* seems that FFmpeg presets have 720p as divider for buffers */
        if( p_enc->fmt_out.video.i_visible_height >= 720 )
        {
            /* Check that we don't overrun users qmin/qmax values */
            if( !var_GetInteger( p_enc, ENC_CFG_PREFIX "qmin" ) )
            {
                p_context->qmin = 10;
                p_context->mb_lmin = p_context->lmin = 10 * FF_QP2LAMBDA;
            }
            if( !var_GetInteger( p_enc, ENC_CFG_PREFIX "qmax" ) )
            {
                p_context->qmax = 42;
                p_context->mb_lmax = p_context->lmax = 42 * FF_QP2LAMBDA;
            }
            } else {
            if( !var_GetInteger( p_enc, ENC_CFG_PREFIX "qmin" ) )
            {
                p_context->qmin = 1;
                p_context->mb_lmin = p_context->lmin = FF_QP2LAMBDA;
            }
        }
#if 0 /* enable when/if vp8 encoder is accepted in libavcodec */
        p_context->lag = 16;
        p_context->level = 216;
        p_context->profile = 0;
        p_context->rc_buffer_aggressivity = 0.95;
        p_context->token_partitions = 4;
        p_context->mb_static_threshold = 0;
#endif
    }
    if( i_codec_id == AV_CODEC_ID_RAWVIDEO )
    {
        /* XXX: hack: Force same codec (will be handled by transcode) */
        p_enc->fmt_in.video.i_chroma = p_enc->fmt_in.i_codec = p_enc->fmt_out.i_codec;
        GetFfmpegChroma( &p_context->pix_fmt, &p_enc->fmt_in.video );
    }
    /* Make sure we get extradata filled by the encoder */
    p_context->extradata_size = 0;
    p_context->extradata = NULL;
    p_context->flags |= CODEC_FLAG_GLOBAL_HEADER;
    if( p_enc->i_threads >= 1)
        p_context->thread_count = p_enc->i_threads;
    else
        p_context->thread_count = vlc_GetCPUCount();
    int ret;
    char *psz_opts = var_InheritString(p_enc, ENC_CFG_PREFIX "options");
    AVDictionary *options = NULL;
    if (psz_opts && *psz_opts)
        options = vlc_av_get_options(psz_opts);
    free(psz_opts);
    vlc_avcodec_lock();
    ret = avcodec_open2( p_context, p_codec, options ? &options : NULL );
    vlc_avcodec_unlock();
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(options, "", t, AV_DICT_IGNORE_SUFFIX))) {
        msg_Err(p_enc, "Unknown option \"%s\"", t->key);
    }
    if( ret )
    {
        if( p_enc->fmt_in.i_cat != AUDIO_ES ||
                (p_context->channels <= 2 && i_codec_id != AV_CODEC_ID_MP2
                 && i_codec_id != AV_CODEC_ID_MP3) )
errmsg:
        {
            static const char types[][12] = {
                [UNKNOWN_ES] = N_("unknown"), [VIDEO_ES] = N_("video"),
                [AUDIO_ES] = N_("audio"), [SPU_ES] = N_("subpicture"),
            };
            const char *type = types[0];
            union
            {
                vlc_fourcc_t value;
                char txt[4];
            } fcc = { .value = p_enc->fmt_out.i_codec };
            if (likely((unsigned)p_enc->fmt_in.i_cat < sizeof (types) / sizeof (types[0])))
                type = types[p_enc->fmt_in.i_cat];
            msg_Err( p_enc, "cannot open %4.4s %s encoder", fcc.txt, type );
            dialog_Fatal( p_enc, _("Streaming / Transcoding failed"),
                          _("VLC could not open the %4.4s %s encoder."),
                          fcc.txt, vlc_gettext(type) );
            av_dict_free(&options);
            goto error;
        }
        if( p_context->channels > 2 )
        {
            p_context->channels = 2;
            p_enc->fmt_in.audio.i_channels = 2; // FIXME
            msg_Warn( p_enc, "stereo mode selected (codec limitation)" );
        }
        if( i_codec_id == AV_CODEC_ID_MP2 || i_codec_id == AV_CODEC_ID_MP3 )
        {
            int i_frequency, i;
            es_format_t *fmt = &p_enc->fmt_out;
            for ( i_frequency = 0; i_frequency < 6; i_frequency++ )
                if ( fmt->audio.i_rate == mpa_freq_tab[i_frequency] )
                    break;
            if ( i_frequency == 6 )
            {
                msg_Err( p_enc, "MPEG audio doesn't support frequency=%d",
                        fmt->audio.i_rate );
                av_dict_free(&options);
                goto error;
            }
            for ( i = 1; i < 14; i++ )
                if (fmt->i_bitrate/1000 <= mpa_bitrate_tab[i_frequency / 3][i])
                    break;
            if (fmt->i_bitrate / 1000 != mpa_bitrate_tab[i_frequency / 3][i])
            {
                msg_Warn( p_enc,
                        "MPEG audio doesn't support bitrate=%d, using %d",
                        fmt->i_bitrate,
                        mpa_bitrate_tab[i_frequency / 3][i] * 1000 );
                fmt->i_bitrate = mpa_bitrate_tab[i_frequency / 3][i] * 1000;
                p_context->bit_rate = fmt->i_bitrate;
            }
        }
        p_context->codec = NULL;
        vlc_avcodec_lock();
        ret = avcodec_open2( p_context, p_codec, options ? &options : NULL );
        vlc_avcodec_unlock();
        if( ret )
            goto errmsg;
    }
    av_dict_free(&options);
    if( i_codec_id == AV_CODEC_ID_FLAC )
    {
        p_enc->fmt_out.i_extra = 4 + 1 + 3 + p_context->extradata_size;
        p_enc->fmt_out.p_extra = malloc( p_enc->fmt_out.i_extra );
        if( p_enc->fmt_out.p_extra )
        {
            uint8_t *p = p_enc->fmt_out.p_extra;
            p[0] = 0x66;    /* f */
            p[1] = 0x4C;    /* L */
            p[2] = 0x61;    /* a */
            p[3] = 0x43;    /* C */
            p[4] = 0x80;    /* streaminfo block, last block before audio */
            p[5] = ( p_context->extradata_size >> 16 ) & 0xff;
            p[6] = ( p_context->extradata_size >>  8 ) & 0xff;
            p[7] = ( p_context->extradata_size       ) & 0xff;
            memcpy( &p[8], p_context->extradata, p_context->extradata_size );
        }
        else
        {
            p_enc->fmt_out.i_extra = 0;
        }
    }
    else
    {
        p_enc->fmt_out.i_extra = p_context->extradata_size;
        if( p_enc->fmt_out.i_extra )
        {
            p_enc->fmt_out.p_extra = malloc( p_enc->fmt_out.i_extra );
            if ( p_enc->fmt_out.p_extra == NULL )
            {
                goto error;
            }
            memcpy( p_enc->fmt_out.p_extra, p_context->extradata,
                    p_enc->fmt_out.i_extra );
        }
    }
    p_context->flags &= ~CODEC_FLAG_GLOBAL_HEADER;
    if( p_enc->fmt_in.i_cat == AUDIO_ES )
    {
        p_enc->fmt_in.i_codec = GetVlcAudioFormat( p_sys->p_context->sample_fmt );
        p_enc->fmt_in.audio.i_bitspersample = aout_BitsPerSample( p_enc->fmt_in.i_codec );
        p_sys->i_sample_bytes = (p_enc->fmt_in.audio.i_bitspersample / 8);
        p_sys->i_frame_size = p_context->frame_size > 1 ?
                                    p_context->frame_size :
                                    FF_MIN_BUFFER_SIZE;
        p_sys->i_buffer_out = av_samples_get_buffer_size(NULL,
                p_sys->p_context->channels, p_sys->i_frame_size,
                p_sys->p_context->sample_fmt, DEFAULT_ALIGN);
        p_sys->p_buffer = av_malloc( p_sys->i_buffer_out );
        if ( unlikely( p_sys->p_buffer == NULL ) )
        {
            goto error;
        }
        p_enc->fmt_out.audio.i_frame_length = p_context->frame_size;
        p_enc->fmt_out.audio.i_blockalign = p_context->block_align;
        p_enc->fmt_out.audio.i_bitspersample = aout_BitsPerSample( p_enc->fmt_out.i_codec );
        //b_variable tells if we can feed any size frames to encoder
        p_sys->b_variable = p_context->frame_size ? false : true;
        if( p_sys->b_planar )
        {
            p_sys->p_interleave_buf = av_malloc( p_sys->i_buffer_out );
            if( unlikely( p_sys->p_interleave_buf == NULL ) )
                goto error;
        }
    }
    p_sys->frame = avcodec_alloc_frame();
    if( !p_sys->frame )
    {
        goto error;
    }
    msg_Dbg( p_enc, "found encoder %s", psz_namecodec );
    p_enc->pf_encode_video = EncodeVideo;
    p_enc->pf_encode_audio = EncodeAudio;
    return VLC_SUCCESS;
error:
    free( p_enc->fmt_out.p_extra );
    av_free( p_sys->p_buffer );
    av_free( p_sys->p_interleave_buf );
    free( p_sys );
    return VLC_ENOMEM;
}

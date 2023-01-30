 *****************************************************************************/
int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys;
    const vlc_fourcc_t fourcc = p_filter->fmt_in.video.i_chroma;
    const vlc_chroma_description_t *chroma = vlc_fourcc_GetChromaDescription( fourcc );
    if( chroma == NULL || chroma->pixel_size > 2 )
    {
notsupp:
        msg_Err( p_filter, "unsupported chroma %4.4s", (char*)&fourcc );
        return VLC_EGENERIC;
    }
    unsigned pixel_size = chroma->pixel_size;
    bool packed = false;
    if( chroma->plane_count != 3 )
    {
        packed = true;
        switch( fourcc )
        {
            case VLC_CODEC_YUYV:
            case VLC_CODEC_UYVY:
            case VLC_CODEC_YVYU:
            case VLC_CODEC_VYUY:
            case VLC_CODEC_NV12:
            case VLC_CODEC_NV21:
                pixel_size = 1;
                break;
            default:
                goto notsupp;
        }
    }
    assert( vlc_fourcc_IsYUV( fourcc ) );
    /* */
    p_sys = p_filter->p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->chroma = chroma;
    config_ChainParse( p_filter, FILTER_CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    char *psz_mode = var_InheritString( p_filter, FILTER_CFG_PREFIX "mode" );
    SetFilterMethod( p_filter, psz_mode, packed );
    free( psz_mode );
    for( int i = 0; i < METADATA_SIZE; i++ )
    {
        p_sys->meta.pi_date[i] = VLC_TS_INVALID;
        p_sys->meta.pi_nb_fields[i] = 2;
        p_sys->meta.pb_top_field_first[i] = true;
    }
    p_sys->i_frame_offset = 0; /* start with default value (first-ever frame
                                  cannot have offset) */
    for( int i = 0; i < HISTORY_SIZE; i++ )
        p_sys->pp_history[i] = NULL;
    IVTCClearState( p_filter );
#if defined(CAN_COMPILE_C_ALTIVEC)
    if( pixel_size == 1 && vlc_CPU_ALTIVEC() )
        p_sys->pf_merge = MergeAltivec;
    else
#endif
#if defined(CAN_COMPILE_SSE2)
    if( vlc_CPU_SSE2() )
    {
        p_sys->pf_merge = pixel_size == 1 ? Merge8BitSSE2 : Merge16BitSSE2;
        p_sys->pf_end_merge = EndMMX;
    }
    else
#endif
#if defined(CAN_COMPILE_MMXEXT)
    if( pixel_size == 1 && vlc_CPU_MMXEXT() )
    {
        p_sys->pf_merge = MergeMMXEXT;
        p_sys->pf_end_merge = EndMMX;
    }
    else
#endif
#if defined(CAN_COMPILE_3DNOW)
    if( pixel_size == 1 && vlc_CPU_3dNOW() )
    {
        p_sys->pf_merge = Merge3DNow;
        p_sys->pf_end_merge = End3DNow;
    }
    else
#endif
#if defined(CAN_COMPILE_ARM)
    if( vlc_CPU_ARM_NEON() )
        p_sys->pf_merge = pixel_size == 1 ? merge8_arm_neon : merge16_arm_neon;
    else
    if( vlc_CPU_ARMv6() )
        p_sys->pf_merge = pixel_size == 1 ? merge8_armv6 : merge16_armv6;
    else
#endif
    {
        p_sys->pf_merge = pixel_size == 1 ? Merge8BitGeneric : Merge16BitGeneric;
#if defined(__i386__) || defined(__x86_64__)
        p_sys->pf_end_merge = NULL;
#endif
    }
    /* */
    if( p_sys->i_mode == DEINTERLACE_PHOSPHOR )
    {
        int i_c420 = var_GetInteger( p_filter,
                                     FILTER_CFG_PREFIX "phosphor-chroma" );
        if( i_c420 != PC_LATEST  &&  i_c420 != PC_ALTLINE  &&
            i_c420 != PC_BLEND   && i_c420 != PC_UPCONVERT )
        {
            msg_Dbg( p_filter, "Phosphor 4:2:0 input chroma mode not set"\
                               "or out of range (valid: 1, 2, 3 or 4), "\
                               "using default" );
            i_c420 = PC_ALTLINE;
        }
        msg_Dbg( p_filter, "using Phosphor 4:2:0 input chroma mode %d",
                           i_c420 );
        /* This maps directly to the phosphor_chroma_t enum. */
        p_sys->phosphor.i_chroma_for_420 = i_c420;
        int i_dimmer = var_GetInteger( p_filter,
                                       FILTER_CFG_PREFIX "phosphor-dimmer" );
        if( i_dimmer < 1  ||  i_dimmer > 4 )
        {
            msg_Dbg( p_filter, "Phosphor dimmer strength not set "\
                               "or out of range (valid: 1, 2, 3 or 4), "\
                               "using default" );
            i_dimmer = 2; /* low */
        }
        msg_Dbg( p_filter, "using Phosphor dimmer strength %d", i_dimmer );
        /* The internal value ranges from 0 to 3. */
        p_sys->phosphor.i_dimmer_strength = i_dimmer - 1;
    }
    else
    {
        p_sys->phosphor.i_chroma_for_420 = PC_ALTLINE;
        p_sys->phosphor.i_dimmer_strength = 1;
    }
    /* */
    video_format_t fmt;
    GetOutputFormat( p_filter, &fmt, &p_filter->fmt_in.video );
    if( !p_filter->b_allow_fmt_out_change &&
        ( fmt.i_chroma != p_filter->fmt_in.video.i_chroma ||
          fmt.i_height != p_filter->fmt_in.video.i_height ) )
    {
        Close( VLC_OBJECT(p_filter) );
        return VLC_EGENERIC;
    }
    p_filter->fmt_out.video = fmt;
    p_filter->fmt_out.i_codec = fmt.i_chroma;
    p_filter->pf_video_filter = Deinterlace;
    p_filter->pf_video_flush  = Flush;
    p_filter->pf_video_mouse  = Mouse;
    msg_Dbg( p_filter, "deinterlacing" );
    return VLC_SUCCESS;
}

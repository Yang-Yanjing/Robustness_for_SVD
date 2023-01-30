}
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if(   p_filter->fmt_in.video.i_chroma != VLC_CODEC_I420
       && p_filter->fmt_in.video.i_chroma != VLC_CODEC_J420
       && p_filter->fmt_in.video.i_chroma != VLC_CODEC_YV12
       && p_filter->fmt_in.video.i_chroma != VLC_CODEC_I422
       && p_filter->fmt_in.video.i_chroma != VLC_CODEC_J422
      )
    {
        /* We only want planar YUV 4:2:0 or 4:2:2 */
        msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                 (char*)&(p_filter->fmt_in.video.i_chroma) );
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    p_filter->pf_video_filter = Filter;
    p_filter->p_sys->f_sigma =
        var_CreateGetFloat( p_filter, FILTER_PREFIX "sigma" );
    if( p_filter->p_sys->f_sigma <= 0. )
    {
        msg_Err( p_filter, "sigma must be greater than zero" );
        return VLC_EGENERIC;
    }
    gaussianblur_InitDistribution( p_filter->p_sys );
    msg_Dbg( p_filter, "gaussian distribution is %d pixels wide",
             p_filter->p_sys->i_dim*2+1 );
    p_filter->p_sys->pt_buffer = NULL;
    p_filter->p_sys->pt_scale = NULL;
    return VLC_SUCCESS;
}

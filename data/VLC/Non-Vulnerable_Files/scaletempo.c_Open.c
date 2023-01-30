 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t *)p_this;
    /* Allocate structure */
    filter_sys_t *p_sys = p_filter->p_sys = malloc( sizeof(*p_sys) );
    if( ! p_sys )
        return VLC_ENOMEM;
    p_sys->scale             = 1.0;
    p_sys->sample_rate       = p_filter->fmt_in.audio.i_rate;
    p_sys->samples_per_frame = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    p_sys->bytes_per_sample  = 4;
    p_sys->bytes_per_frame   = p_sys->samples_per_frame * p_sys->bytes_per_sample;
    msg_Dbg( p_this, "format: %5i rate, %i nch, %i bps, %s",
             p_sys->sample_rate,
             p_sys->samples_per_frame,
             p_sys->bytes_per_sample,
             "fl32" );
    p_sys->ms_stride       = var_InheritInteger( p_this, "scaletempo-stride" );
    p_sys->percent_overlap = var_InheritFloat( p_this, "scaletempo-overlap" );
    p_sys->ms_search       = var_InheritInteger( p_this, "scaletempo-search" );
    msg_Dbg( p_this, "params: %i stride, %.3f overlap, %i search",
             p_sys->ms_stride, p_sys->percent_overlap, p_sys->ms_search );
    p_sys->buf_queue      = NULL;
    p_sys->buf_overlap    = NULL;
    p_sys->table_blend    = NULL;
    p_sys->buf_pre_corr   = NULL;
    p_sys->table_window   = NULL;
    p_sys->bytes_overlap  = 0;
    p_sys->bytes_queued   = 0;
    p_sys->bytes_to_slide = 0;
    p_sys->frames_stride_error = 0;
    if( reinit_buffers( p_filter ) != VLC_SUCCESS )
    {
        Close( p_this );
        return VLC_EGENERIC;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}

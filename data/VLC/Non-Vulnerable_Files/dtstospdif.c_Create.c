 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    if( p_filter->fmt_in.audio.i_format != VLC_CODEC_DTS ||
        ( p_filter->fmt_out.audio.i_format != VLC_CODEC_SPDIFL &&
          p_filter->fmt_out.audio.i_format != VLC_CODEC_SPDIFB ) )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->p_buf = NULL;
    p_sys->i_frame_size = 0;
    p_sys->i_frames = 0;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}

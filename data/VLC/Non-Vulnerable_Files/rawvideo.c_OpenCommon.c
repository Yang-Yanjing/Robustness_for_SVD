 */
static int OpenCommon( decoder_t *p_dec )
{
    const vlc_chroma_description_t *dsc =
        vlc_fourcc_GetChromaDescription( p_dec->fmt_in.i_codec );
    if( dsc == NULL || dsc->plane_count == 0 )
        return VLC_EGENERIC;
    if( p_dec->fmt_in.video.i_width <= 0 || p_dec->fmt_in.video.i_height == 0 )
    {
        msg_Err( p_dec, "invalid display size %dx%d",
                 p_dec->fmt_in.video.i_width, p_dec->fmt_in.video.i_height );
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    decoder_sys_t *p_sys = calloc(1, sizeof(*p_sys));
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    if( (int)p_dec->fmt_in.video.i_height < 0 )
    {
        /* Frames are coded from bottom to top */
        p_dec->fmt_in.video.i_height =
            (unsigned int)(-(int)p_dec->fmt_in.video.i_height);
        p_sys->b_invert = true;
    }
    if( !p_dec->fmt_in.video.i_visible_width )
        p_dec->fmt_in.video.i_visible_width = p_dec->fmt_in.video.i_width;
    if( !p_dec->fmt_in.video.i_visible_height )
        p_dec->fmt_in.video.i_visible_height = p_dec->fmt_in.video.i_height;
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    date_Init( &p_sys->pts, p_dec->fmt_out.video.i_frame_rate,
               p_dec->fmt_out.video.i_frame_rate_base );
    if( p_dec->fmt_out.video.i_frame_rate == 0 ||
        p_dec->fmt_out.video.i_frame_rate_base == 0)
    {
        msg_Warn( p_dec, "invalid frame rate %d/%d, using 25 fps instead",
                  p_dec->fmt_out.video.i_frame_rate,
                  p_dec->fmt_out.video.i_frame_rate_base);
        date_Init( &p_sys->pts, 25, 1 );
    }
    for( unsigned i = 0; i < dsc->plane_count; i++ )
    {
        unsigned pitch = p_dec->fmt_in.video.i_width * dsc->pixel_size
                         * dsc->p[i].w.num / dsc->p[i].w.den;
        unsigned lines = p_dec->fmt_in.video.i_height
                         * dsc->p[i].h.num / dsc->p[i].h.den;
        p_sys->pitches[i] = pitch;
        p_sys->lines[i] = lines;
        p_sys->size += pitch * lines;
    }
    p_dec->p_sys           = p_sys;
    return VLC_SUCCESS;
}

 *****************************************************************************/
static picture_t *GetNewPicture( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    picture_t *p_pic;
    p_dec->fmt_out.video.i_width = p_sys->p_info->sequence->width;
    p_dec->fmt_out.video.i_visible_width =
        p_sys->p_info->sequence->picture_width;
    p_dec->fmt_out.video.i_height = p_sys->p_info->sequence->height;
    p_dec->fmt_out.video.i_visible_height =
        p_sys->p_info->sequence->picture_height;
    p_dec->fmt_out.video.i_sar_num = p_sys->i_sar_num;
    p_dec->fmt_out.video.i_sar_den = p_sys->i_sar_den;
    if( p_sys->p_info->sequence->frame_period > 0 )
    {
        p_dec->fmt_out.video.i_frame_rate =
            (uint32_t)( (uint64_t)1001000000 * 27 /
                        p_sys->p_info->sequence->frame_period );
        p_dec->fmt_out.video.i_frame_rate_base = 1001;
    }
    p_dec->fmt_out.i_codec =
        ( p_sys->p_info->sequence->chroma_height <
          p_sys->p_info->sequence->height ) ?
        VLC_CODEC_I420 : VLC_CODEC_I422;
    /* Get a new picture */
    p_pic = decoder_NewPicture( p_dec );
    if( p_pic == NULL )
        return NULL;
    p_pic->b_progressive = p_sys->p_info->current_picture != NULL ?
        p_sys->p_info->current_picture->flags & PIC_FLAG_PROGRESSIVE_FRAME : 1;
    p_pic->b_top_field_first = p_sys->p_info->current_picture != NULL ?
        p_sys->p_info->current_picture->flags & PIC_FLAG_TOP_FIELD_FIRST : 1;
    p_pic->i_nb_fields = p_sys->p_info->current_picture != NULL ?
        p_sys->p_info->current_picture->nb_fields : 2;
    return p_pic;
}

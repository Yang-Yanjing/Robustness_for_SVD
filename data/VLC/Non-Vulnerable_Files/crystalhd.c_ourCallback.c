}
static BC_STATUS ourCallback(void *shnd, uint32_t width, uint32_t height, uint32_t stride, void *pOut)
{
    VLC_UNUSED(width); VLC_UNUSED(height); VLC_UNUSED(stride);
    decoder_t *p_dec          = (decoder_t *)shnd;
    BC_DTS_PROC_OUT *proc_out = p_dec->p_sys->proc_out;
    BC_DTS_PROC_OUT *proc_in  = (BC_DTS_PROC_OUT*)pOut;
    /* Direct Rendering */
    /* Do not allocate for the second-field in the pair, in interlaced */
    if( !(proc_in->PicInfo.flags & VDEC_FLAG_INTERLACED_SRC) ||
        !(proc_in->PicInfo.flags & VDEC_FLAG_FIELDPAIR) )
        p_dec->p_sys->p_pic = decoder_NewPicture( p_dec );
    /* */
    picture_t *p_pic = p_dec->p_sys->p_pic;
    if( !p_pic )
        return BC_STS_ERROR;
    /* Interlacing */
    p_pic->b_progressive     = !(proc_in->PicInfo.flags & VDEC_FLAG_INTERLACED_SRC);
    p_pic->b_top_field_first = !(proc_in->PicInfo.flags & VDEC_FLAG_BOTTOM_FIRST);
    p_pic->i_nb_fields       = p_pic->b_progressive? 1: 2;
    /* Filling out the struct */
    proc_out->Ybuff      = !(proc_in->PicInfo.flags & VDEC_FLAG_FIELDPAIR) ?
                             &p_pic->p[0].p_pixels[0] :
                             &p_pic->p[0].p_pixels[p_pic->p[0].i_pitch];
    proc_out->YbuffSz    = 2 * p_pic->p[0].i_pitch;
    proc_out->StrideSz   = (proc_in->PicInfo.flags & VDEC_FLAG_INTERLACED_SRC)?
                            2 * (p_pic->p[0].i_pitch/2) - p_dec->fmt_out.video.i_width:
                            p_pic->p[0].i_pitch/2 - p_dec->fmt_out.video.i_width;
    proc_out->PoutFlags |= BC_POUT_FLAGS_STRIDE;              /* Trust Stride info */
    return BC_STS_SUCCESS;
}

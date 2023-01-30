 */
static picture_t *DecodeBlock(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    picture_t *p_pic = 0;
    JSAMPARRAY p_row_pointers = NULL;
    if (!pp_block || !*pp_block)
    {
        return NULL;
    }
    p_block = *pp_block;
    if (p_block->i_flags & BLOCK_FLAG_DISCONTINUITY)
    {
        block_Release(p_block);
        *pp_block = NULL;
        return NULL;
    }
    /* libjpeg longjmp's there in case of error */
    if (setjmp(p_sys->setjmp_buffer))
    {
        goto error;
    }
    jpeg_create_decompress(&p_sys->p_jpeg);
    jpeg_mem_src(&p_sys->p_jpeg, p_block->p_buffer, p_block->i_buffer);
    jpeg_read_header(&p_sys->p_jpeg, TRUE);
    p_sys->p_jpeg.out_color_space = JCS_RGB;
    jpeg_start_decompress(&p_sys->p_jpeg);
    /* Set output properties */
    p_dec->fmt_out.i_codec = VLC_CODEC_RGB24;
    p_dec->fmt_out.video.i_visible_width  = p_dec->fmt_out.video.i_width  = p_sys->p_jpeg.output_width;
    p_dec->fmt_out.video.i_visible_height = p_dec->fmt_out.video.i_height = p_sys->p_jpeg.output_height;
    p_dec->fmt_out.video.i_sar_num = 1;
    p_dec->fmt_out.video.i_sar_den = 1;
    p_dec->fmt_out.video.i_rmask = 0x000000ff;
    p_dec->fmt_out.video.i_gmask = 0x0000ff00;
    p_dec->fmt_out.video.i_bmask = 0x00ff0000;
    /* Get a new picture */
    p_pic = decoder_NewPicture(p_dec);
    if (!p_pic)
    {
        goto error;
    }
    /* Decode picture */
    p_row_pointers = malloc(sizeof(JSAMPROW) * p_sys->p_jpeg.output_height);
    if (!p_row_pointers)
    {
        goto error;
    }
    for (unsigned i = 0; i < p_sys->p_jpeg.output_height; i++) {
        p_row_pointers[i] = p_pic->p->p_pixels + p_pic->p->i_pitch * i;
    }
    while (p_sys->p_jpeg.output_scanline < p_sys->p_jpeg.output_height)
    {
        jpeg_read_scanlines(&p_sys->p_jpeg,
                p_row_pointers + p_sys->p_jpeg.output_scanline,
                p_sys->p_jpeg.output_height - p_sys->p_jpeg.output_scanline);
    }
    jpeg_finish_decompress(&p_sys->p_jpeg);
    jpeg_destroy_decompress(&p_sys->p_jpeg);
    free(p_row_pointers);
    p_pic->date = p_block->i_pts > VLC_TS_INVALID ? p_block->i_pts : p_block->i_dts;
    block_Release(p_block);
    *pp_block = NULL;
    return p_pic;
error:
    jpeg_destroy_decompress(&p_sys->p_jpeg);
    free(p_row_pointers);
    block_Release(p_block);
    *pp_block = NULL;
    return NULL;
}

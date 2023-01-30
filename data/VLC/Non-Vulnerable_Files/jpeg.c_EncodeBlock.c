 */
static block_t *EncodeBlock(encoder_t *p_enc, picture_t *p_pic)
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    if (unlikely(!p_pic))
    {
        return NULL;
    }
    block_t *p_block = block_Alloc(p_sys->i_blocksize);
    if (p_block == NULL)
    {
        return NULL;
    }
    JSAMPIMAGE p_row_pointers = NULL;
    /* libjpeg longjmp's there in case of error */
    if (setjmp(p_sys->setjmp_buffer))
    {
        goto error;
    }
    jpeg_create_compress(&p_sys->p_jpeg);
    jpeg_mem_dest(&p_sys->p_jpeg, &p_block->p_buffer, &p_block->i_buffer);
    p_sys->p_jpeg.image_width = p_enc->fmt_in.video.i_visible_width;
    p_sys->p_jpeg.image_height = p_enc->fmt_in.video.i_visible_height;
    p_sys->p_jpeg.input_components = 3;
    p_sys->p_jpeg.in_color_space = JCS_YCbCr;
    jpeg_set_defaults(&p_sys->p_jpeg);
    jpeg_set_colorspace(&p_sys->p_jpeg, JCS_YCbCr);
    p_sys->p_jpeg.raw_data_in = TRUE;
#if JPEG_LIB_VERSION >= 70
    p_sys->p_jpeg.do_fancy_downsampling = FALSE;
#endif
    jpeg_set_quality(&p_sys->p_jpeg, p_sys->i_quality, TRUE);
    jpeg_start_compress(&p_sys->p_jpeg, TRUE);
    /* Encode picture */
    p_row_pointers = malloc(sizeof(JSAMPARRAY) * p_pic->i_planes);
    if (p_row_pointers == NULL)
    {
        goto error;
    }
    for (int i = 0; i < p_pic->i_planes; i++)
    {
        p_row_pointers[i] = malloc(sizeof(JSAMPROW) * p_sys->p_jpeg.comp_info[i].v_samp_factor * DCTSIZE);
    }
    while (p_sys->p_jpeg.next_scanline < p_sys->p_jpeg.image_height)
    {
        for (int i = 0; i < p_pic->i_planes; i++)
        {
            int i_offset = p_sys->p_jpeg.next_scanline * p_sys->p_jpeg.comp_info[i].v_samp_factor / p_sys->p_jpeg.max_v_samp_factor;
            for (int j = 0; j < p_sys->p_jpeg.comp_info[i].v_samp_factor * DCTSIZE; j++)
            {
                p_row_pointers[i][j] = p_pic->p[i].p_pixels + p_pic->p[i].i_pitch * (i_offset + j);
            }
        }
        jpeg_write_raw_data(&p_sys->p_jpeg, p_row_pointers, p_sys->p_jpeg.max_v_samp_factor * DCTSIZE);
    }
    jpeg_finish_compress(&p_sys->p_jpeg);
    jpeg_destroy_compress(&p_sys->p_jpeg);
    for (int i = 0; i < p_pic->i_planes; i++)
    {
        free(p_row_pointers[i]);
    }
    free(p_row_pointers);
    p_block->i_dts = p_block->i_pts = p_pic->date;
    return p_block;
error:
    jpeg_destroy_compress(&p_sys->p_jpeg);
    if (p_row_pointers != NULL)
    {
        for (int i = 0; i < p_pic->i_planes; i++)
        {
            free(p_row_pointers[i]);
        }
    }
    free(p_row_pointers);
    block_Release(p_block);
    return NULL;
}

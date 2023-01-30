 ****************************************************************************/
static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    picture_t *p_pic = 0;
    png_uint_32 i_width, i_height;
    int i_color_type, i_interlace_type, i_compression_type, i_filter_type;
    int i_bit_depth, i;
    png_structp p_png;
    png_infop p_info, p_end_info;
    png_bytep *p_row_pointers = NULL;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    p_sys->b_error = false;
    if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
    {
        block_Release( p_block ); *pp_block = NULL;
        return NULL;
    }
    p_png = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    if( p_png == NULL )
    {
        block_Release( p_block ); *pp_block = NULL;
        return NULL;
    }
    p_info = png_create_info_struct( p_png );
    if( p_info == NULL )
    {
        png_destroy_read_struct( &p_png, NULL, NULL );
        block_Release( p_block ); *pp_block = NULL;
        return NULL;
    }
    p_end_info = png_create_info_struct( p_png );
    if( p_end_info == NULL )
    {
        png_destroy_read_struct( &p_png, &p_info, NULL );
        block_Release( p_block ); *pp_block = NULL;
        return NULL;
    }
    /* libpng longjmp's there in case of error */
    if( setjmp( png_jmpbuf( p_png ) ) )
        goto error;
    png_set_read_fn( p_png, (void *)p_block, user_read );
    png_set_error_fn( p_png, (void *)p_dec, user_error, user_warning );
    png_read_info( p_png, p_info );
    if( p_sys->b_error ) goto error;
    png_get_IHDR( p_png, p_info, &i_width, &i_height,
                  &i_bit_depth, &i_color_type, &i_interlace_type,
                  &i_compression_type, &i_filter_type);
    if( p_sys->b_error ) goto error;
    /* Set output properties */
    p_dec->fmt_out.i_codec = VLC_CODEC_RGBA;
    p_dec->fmt_out.video.i_visible_width = p_dec->fmt_out.video.i_width = i_width;
    p_dec->fmt_out.video.i_visible_height = p_dec->fmt_out.video.i_height = i_height;
    p_dec->fmt_out.video.i_sar_num = 1;
    p_dec->fmt_out.video.i_sar_den = 1;
    p_dec->fmt_out.video.i_rmask = 0x000000ff;
    p_dec->fmt_out.video.i_gmask = 0x0000ff00;
    p_dec->fmt_out.video.i_bmask = 0x00ff0000;
    if( i_color_type == PNG_COLOR_TYPE_PALETTE )
        png_set_palette_to_rgb( p_png );
    if( i_color_type == PNG_COLOR_TYPE_GRAY ||
        i_color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
          png_set_gray_to_rgb( p_png );
    /* Strip to 8 bits per channel */
    if( i_bit_depth == 16 ) png_set_strip_16( p_png );
    if( png_get_valid( p_png, p_info, PNG_INFO_tRNS ) )
    {
        png_set_tRNS_to_alpha( p_png );
    }
    else if( !(i_color_type & PNG_COLOR_MASK_ALPHA) )
    {
        p_dec->fmt_out.i_codec = VLC_CODEC_RGB24;
    }
    /* Get a new picture */
    p_pic = decoder_NewPicture( p_dec );
    if( !p_pic ) goto error;
    /* Decode picture */
    p_row_pointers = malloc( sizeof(png_bytep) * i_height );
    if( !p_row_pointers )
        goto error;
    for( i = 0; i < (int)i_height; i++ )
        p_row_pointers[i] = p_pic->p->p_pixels + p_pic->p->i_pitch * i;
    png_read_image( p_png, p_row_pointers );
    if( p_sys->b_error ) goto error;
    png_read_end( p_png, p_end_info );
    if( p_sys->b_error ) goto error;
    png_destroy_read_struct( &p_png, &p_info, &p_end_info );
    free( p_row_pointers );
    p_pic->date = p_block->i_pts > VLC_TS_INVALID ? p_block->i_pts : p_block->i_dts;
    block_Release( p_block ); *pp_block = NULL;
    return p_pic;
 error:
    free( p_row_pointers );
    png_destroy_read_struct( &p_png, &p_info, &p_end_info );
    block_Release( p_block ); *pp_block = NULL;
    return NULL;
}

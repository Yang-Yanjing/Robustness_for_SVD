 *****************************************************************************/
static subpicture_t *SetupSimpleKateSPU( decoder_t *p_dec, subpicture_t *p_spu,
                                         const kate_event *ev )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    video_format_t fmt;
    subpicture_region_t *p_bitmap_region = NULL;
    video_palette_t palette;
    kate_tracker kin;
    bool b_tracker_valid = false;
    int i_ret;
    /* these may be 0 for "not specified" */
    p_spu->i_original_picture_width = p_sys->ki.original_canvas_width;
    p_spu->i_original_picture_height = p_sys->ki.original_canvas_height;
    /* Create a new subpicture region */
    memset( &fmt, 0, sizeof(video_format_t) );
    if (p_sys->b_formatted)
    {
        i_ret = kate_tracker_init( &kin, &p_sys->ki, ev );
        if( i_ret < 0)
        {
            msg_Err( p_dec, "failed to initialize kate tracker, event will be unformatted: %d", i_ret );
        }
        else
        {
            int w = 720, h = 576; /* give sensible defaults just in case we fail to get the actual size */
            GetVideoSize(p_dec, &w, &h);
            i_ret = kate_tracker_update(&kin, 0, w, h, 0, 0, w, h);
            if( i_ret < 0)
            {
                kate_tracker_clear(&kin);
                msg_Err( p_dec, "failed to update kate tracker, event will be unformatted: %d", i_ret );
            }
            else
            {
                // TODO: parse tracker and set style, init fmt
                b_tracker_valid = true;
            }
        }
    }
    if (ev->bitmap && ev->bitmap->type==kate_bitmap_type_paletted && ev->palette) {
        /* create a separate region for the bitmap */
        memset( &fmt, 0, sizeof(video_format_t) );
        fmt.i_chroma = VLC_CODEC_YUVP;
        fmt.i_width = fmt.i_visible_width = ev->bitmap->width;
        fmt.i_height = fmt.i_visible_height = ev->bitmap->height;
        fmt.i_x_offset = fmt.i_y_offset = 0;
        fmt.p_palette = &palette;
        CreateKatePalette( fmt.p_palette, ev->palette );
        p_bitmap_region = subpicture_region_New( &fmt );
        if( !p_bitmap_region )
        {
            msg_Err( p_dec, "cannot allocate SPU region" );
            decoder_DeleteSubpicture( p_dec, p_spu );
            return NULL;
        }
        /* create the bitmap */
        CreateKateBitmap( p_bitmap_region->p_picture, ev->bitmap );
        msg_Dbg(p_dec, "Created bitmap, %zux%zu, %zu colors", ev->bitmap->width, ev->bitmap->height, ev->palette->ncolors);
    }
    /* text region */
    fmt.i_chroma = VLC_CODEC_TEXT;
    fmt.i_sar_num = 0;
    fmt.i_sar_den = 1;
    fmt.i_width = fmt.i_height = 0;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    p_spu->p_region = subpicture_region_New( &fmt );
    if( !p_spu->p_region )
    {
        msg_Err( p_dec, "cannot allocate SPU region" );
        if( p_bitmap_region )
            subpicture_region_Delete( p_bitmap_region );
        decoder_DeleteSubpicture( p_dec, p_spu );
        return NULL;
    }
    SetupText( p_dec, p_spu, ev );
    /* default positioning */
    p_spu->p_region->i_align = SUBPICTURE_ALIGN_BOTTOM;
    if (p_bitmap_region)
    {
        p_bitmap_region->i_align = SUBPICTURE_ALIGN_BOTTOM;
    }
    p_spu->p_region->i_x = 0;
    p_spu->p_region->i_y = 10;
    /* override if tracker info present */
    if (b_tracker_valid)
    {
        if (kin.has.region)
        {
            p_spu->p_region->i_x = kin.region_x;
            p_spu->p_region->i_y = kin.region_y;
            if (p_bitmap_region)
            {
                p_bitmap_region->i_x = kin.region_x;
                p_bitmap_region->i_y = kin.region_y;
            }
            p_spu->b_absolute = true;
        }
        kate_tracker_clear(&kin);
    }
    /* if we have a bitmap, chain it before the text */
    if (p_bitmap_region)
    {
        p_bitmap_region->p_next = p_spu->p_region;
        p_spu->p_region = p_bitmap_region;
    }
    return p_spu;
}

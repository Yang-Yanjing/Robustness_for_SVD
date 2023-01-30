}
static subpicture_t *render( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t *p_spu;
    subpicture_region_t **pp_spu_region;
    int i, j;
    int i_base_x;
    int i_base_y;
    /* Allocate the subpicture internal data. */
    p_spu = decoder_NewSubpicture( p_dec, NULL );
    if( !p_spu )
        return NULL;
    p_spu->b_absolute = p_sys->b_absolute;
    /* Set the pf_render callback */
    p_spu->i_start = p_sys->i_pts;
    //p_spu->i_stop = (mtime_t) 0;
    p_spu->b_ephemer = true;
    //p_spu->b_fade = true;
    //p_spu->i_stop = p_spu->i_start + (mtime_t) (i_timeout * 1000000);
    p_spu->b_subtitle = true;
    /* Correct positioning of SPU */
    i_base_x = p_sys->i_spu_x;
    i_base_y = p_sys->i_spu_y;
    p_spu->i_original_picture_width = 720;
    p_spu->i_original_picture_height = 576;
    p_spu->i_original_picture_width = p_sys->display.i_width;
    p_spu->i_original_picture_height = p_sys->display.i_height;
    if( p_sys->display.b_windowed )
    {
        /* From en_300743v01 - */
        /* the DDS is there to indicate intended size/position of text */
        /* the intended video area is ->i_width/height */
        /* the window is within this... SO... we should leave i_original_picture_width etc. as is */
        /* and ONLY change i_base_x.  effectively i_max_x/y are only there to limit memory requirements*/
        /* we COULD use the upper limits to limit rendering to within these? */
        /* see notes on DDS at the top of the file */
        i_base_x += p_sys->display.i_x;
        i_base_y += p_sys->display.i_y;
    }
    pp_spu_region = &p_spu->p_region;
    /* Loop on region definitions */
#ifdef DEBUG_DVBSUB
    if( p_sys->p_page )
        msg_Dbg( p_dec, "rendering %i regions", p_sys->p_page->i_region_defs );
#endif
    for( i = 0; p_sys->p_page && ( i < p_sys->p_page->i_region_defs ); i++ )
    {
        dvbsub_region_t     *p_region;
        dvbsub_regiondef_t  *p_regiondef;
        dvbsub_clut_t       *p_clut;
        dvbsub_color_t      *p_color;
        subpicture_region_t *p_spu_region;
        uint8_t *p_src, *p_dst;
        video_format_t fmt;
        video_palette_t palette;
        int i_pitch;
        p_regiondef = &p_sys->p_page->p_region_defs[i];
        /* Find associated region */
        for( p_region = p_sys->p_regions; p_region != NULL;
             p_region = p_region->p_next )
        {
            if( p_regiondef->i_id == p_region->i_id ) break;
        }
#ifdef DEBUG_DVBSUB
        /* if a region exists, then print it's size */
        if (p_region)
        {
                msg_Dbg( p_dec, "rendering region %i (%i,%i) to (%i,%i)", i,
                        p_regiondef->i_x, p_regiondef->i_y,
                p_regiondef->i_x + p_region->i_width,
                p_regiondef->i_y + p_region->i_height );
        }
        else
        {
                msg_Dbg( p_dec, "rendering region %i (%i,%i) (no region matched to render)", i,
                      p_regiondef->i_x, p_regiondef->i_y );
        }
#endif
        if( !p_region )
        {
            msg_Dbg( p_dec, "region %i not found", p_regiondef->i_id );
            continue;
        }
        /* Find associated CLUT */
        for( p_clut = p_sys->p_cluts; p_clut != NULL; p_clut = p_clut->p_next )
        {
            if( p_region->i_clut == p_clut->i_id ) break;
        }
        if( !p_clut )
        {
            msg_Dbg( p_dec, "clut %i not found", p_region->i_clut );
            continue;
        }
        /* FIXME: don't create a subpicture region with VLC CODEC YUVP
         * when it actually is a TEXT region */
        /* Create new SPU region */
        memset( &fmt, 0, sizeof(video_format_t) );
        fmt.i_chroma = VLC_CODEC_YUVP;
        fmt.i_sar_num = 0; /* 0 means use aspect ratio of background video */
        fmt.i_sar_den = 1;
        fmt.i_width = fmt.i_visible_width = p_region->i_width;
        fmt.i_height = fmt.i_visible_height = p_region->i_height;
        fmt.i_x_offset = fmt.i_y_offset = 0;
        fmt.p_palette = &palette;
        fmt.p_palette->i_entries = ( p_region->i_depth == 1 ) ? 4 :
            ( ( p_region->i_depth == 2 ) ? 16 : 256 );
        p_color = ( p_region->i_depth == 1 ) ? p_clut->c_2b :
            ( ( p_region->i_depth == 2 ) ? p_clut->c_4b : p_clut->c_8b );
        for( j = 0; j < fmt.p_palette->i_entries; j++ )
        {
            fmt.p_palette->palette[j][0] = p_color[j].Y;
            fmt.p_palette->palette[j][1] = p_color[j].Cb; /* U == Cb */
            fmt.p_palette->palette[j][2] = p_color[j].Cr; /* V == Cr */
            fmt.p_palette->palette[j][3] = 0xff - p_color[j].T;
        }
        p_spu_region = subpicture_region_New( &fmt );
        if( !p_spu_region )
        {
            msg_Err( p_dec, "cannot allocate SPU region" );
            continue;
        }
        p_spu_region->i_x = i_base_x + p_regiondef->i_x;
        p_spu_region->i_y = i_base_y + p_regiondef->i_y;
        p_spu_region->i_align = p_sys->i_spu_position;
        *pp_spu_region = p_spu_region;
        pp_spu_region = &p_spu_region->p_next;
        p_src = p_region->p_pixbuf;
        p_dst = p_spu_region->p_picture->Y_PIXELS;
        i_pitch = p_spu_region->p_picture->Y_PITCH;
        /* Copy pixel buffer */
        for( j = 0; j < p_region->i_height; j++ )
        {
            memcpy( p_dst, p_src, p_region->i_width );
            p_src += p_region->i_width;
            p_dst += i_pitch;
        }
        /* Check subtitles encoded as strings of characters
         * (since there are not rendered in the pixbuffer) */
        for( j = 0; j < p_region->i_object_defs; j++ )
        {
            dvbsub_objectdef_t *p_object_def = &p_region->p_object_defs[j];
            if( ( p_object_def->i_type != 1 ) || !p_object_def->psz_text )
                continue;
            /* Create new SPU region */
            memset( &fmt, 0, sizeof(video_format_t) );
            fmt.i_chroma = VLC_CODEC_TEXT;
            fmt.i_sar_num = 1;
            fmt.i_sar_den = 1;
            fmt.i_width = fmt.i_visible_width = p_region->i_width;
            fmt.i_height = fmt.i_visible_height = p_region->i_height;
            fmt.i_x_offset = fmt.i_y_offset = 0;
            p_spu_region = subpicture_region_New( &fmt );
            p_spu_region->psz_text = strdup( p_object_def->psz_text );
            p_spu_region->i_x = i_base_x + p_regiondef->i_x + p_object_def->i_x;
            p_spu_region->i_y = i_base_y + p_regiondef->i_y + p_object_def->i_y;
            p_spu_region->i_align = p_sys->i_spu_position;
            *pp_spu_region = p_spu_region;
            pp_spu_region = &p_spu_region->p_next;
        }
    }
    return p_spu;
}

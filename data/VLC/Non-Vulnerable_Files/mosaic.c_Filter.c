 *****************************************************************************/
static subpicture_t *Filter( filter_t *p_filter, mtime_t date )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    bridge_t *p_bridge;
    int i_index, i_real_index, i_row, i_col;
    int i_greatest_real_index_used = p_sys->i_order_length - 1;
    unsigned int col_inner_width, row_inner_height;
    subpicture_region_t *p_region;
    subpicture_region_t *p_region_prev = NULL;
    /* Allocate the subpicture internal data. */
    subpicture_t *p_spu = filter_NewSubpicture( p_filter );
    if( !p_spu )
        return NULL;
    /* Initialize subpicture */
    p_spu->i_channel = 0;
    p_spu->i_start  = date;
    p_spu->i_stop = 0;
    p_spu->b_ephemer = true;
    p_spu->i_alpha = p_sys->i_alpha;
    p_spu->b_absolute = false;
    p_spu->i_original_picture_width = p_sys->i_width;
    p_spu->i_original_picture_height = p_sys->i_height;
    vlc_mutex_lock( &p_sys->lock );
    vlc_global_lock( VLC_MOSAIC_MUTEX );
    p_bridge = GetBridge( p_filter );
    if ( p_bridge == NULL )
    {
        vlc_global_unlock( VLC_MOSAIC_MUTEX );
        vlc_mutex_unlock( &p_sys->lock );
        return p_spu;
    }
    if ( p_sys->i_position == position_offsets )
    {
        /* If we have either too much or not enough offsets, fall-back
         * to automatic positioning. */
        if ( p_sys->i_offsets_length != p_sys->i_order_length )
        {
            msg_Err( p_filter,
                     "Number of specified offsets (%d) does not match number "
                     "of input substreams in mosaic-order (%d), falling back "
                     "to mosaic-position=0",
                     p_sys->i_offsets_length, p_sys->i_order_length );
            p_sys->i_position = position_auto;
        }
    }
    if ( p_sys->i_position == position_auto )
    {
        int i_numpics = p_sys->i_order_length; /* keep slots and all */
        for ( i_index = 0; i_index < p_bridge->i_es_num; i_index++ )
        {
            bridged_es_t *p_es = p_bridge->pp_es[i_index];
            if ( !p_es->b_empty )
            {
                i_numpics ++;
                if( p_sys->i_order_length && p_es->psz_id != NULL )
                {
                    /* We also want to leave slots for images given in
                     * mosaic-order that are not available in p_vout_picture */
                    int i;
                    for( i = 0; i < p_sys->i_order_length ; i++ )
                    {
                        if( !strcmp( p_sys->ppsz_order[i], p_es->psz_id ) )
                        {
                            i_numpics--;
                            break;
                        }
                    }
                }
            }
        }
        p_sys->i_rows = ceil(sqrt( (double)i_numpics ));
        p_sys->i_cols = ( i_numpics % p_sys->i_rows == 0 ?
                            i_numpics / p_sys->i_rows :
                            i_numpics / p_sys->i_rows + 1 );
    }
    col_inner_width  = ( ( p_sys->i_width - ( p_sys->i_cols - 1 )
                       * p_sys->i_borderw ) / p_sys->i_cols );
    row_inner_height = ( ( p_sys->i_height - ( p_sys->i_rows - 1 )
                       * p_sys->i_borderh ) / p_sys->i_rows );
    i_real_index = 0;
    for ( i_index = 0; i_index < p_bridge->i_es_num; i_index++ )
    {
        bridged_es_t *p_es = p_bridge->pp_es[i_index];
        video_format_t fmt_in, fmt_out;
        picture_t *p_converted;
        memset( &fmt_in, 0, sizeof( video_format_t ) );
        memset( &fmt_out, 0, sizeof( video_format_t ) );
        if ( p_es->b_empty )
            continue;
        while ( p_es->p_picture != NULL
                 && p_es->p_picture->date + p_sys->i_delay < date )
        {
            if ( p_es->p_picture->p_next != NULL )
            {
                picture_t *p_next = p_es->p_picture->p_next;
                picture_Release( p_es->p_picture );
                p_es->p_picture = p_next;
            }
            else if ( p_es->p_picture->date + p_sys->i_delay + BLANK_DELAY <
                        date )
            {
                /* Display blank */
                picture_Release( p_es->p_picture );
                p_es->p_picture = NULL;
                p_es->pp_last = &p_es->p_picture;
                break;
            }
            else
            {
                msg_Dbg( p_filter, "too late picture for %s (%"PRId64 ")",
                         p_es->psz_id,
                         date - p_es->p_picture->date - p_sys->i_delay );
                break;
            }
        }
        if ( p_es->p_picture == NULL )
            continue;
        if ( p_sys->i_order_length == 0 )
        {
            i_real_index++;
        }
        else
        {
            int i;
            for ( i = 0; i <= p_sys->i_order_length; i++ )
            {
                if ( i == p_sys->i_order_length ) break;
                if ( strcmp( p_es->psz_id, p_sys->ppsz_order[i] ) == 0 )
                {
                    i_real_index = i;
                    break;
                }
            }
            if ( i == p_sys->i_order_length )
                i_real_index = ++i_greatest_real_index_used;
        }
        i_row = ( i_real_index / p_sys->i_cols ) % p_sys->i_rows;
        i_col = i_real_index % p_sys->i_cols ;
        if ( !p_sys->b_keep )
        {
            /* Convert the images */
            fmt_in.i_chroma = p_es->p_picture->format.i_chroma;
            fmt_in.i_height = p_es->p_picture->format.i_height;
            fmt_in.i_width = p_es->p_picture->format.i_width;
            if( fmt_in.i_chroma == VLC_CODEC_YUVA ||
                fmt_in.i_chroma == VLC_CODEC_RGBA )
                fmt_out.i_chroma = VLC_CODEC_YUVA;
            else
                fmt_out.i_chroma = VLC_CODEC_I420;
            fmt_out.i_width = col_inner_width;
            fmt_out.i_height = row_inner_height;
            if( p_sys->b_ar ) /* keep aspect ratio */
            {
                if( (float)fmt_out.i_width / (float)fmt_out.i_height
                      > (float)fmt_in.i_width / (float)fmt_in.i_height )
                {
                    fmt_out.i_width = ( fmt_out.i_height * fmt_in.i_width )
                                         / fmt_in.i_height;
                }
                else
                {
                    fmt_out.i_height = ( fmt_out.i_width * fmt_in.i_height )
                                        / fmt_in.i_width;
                }
             }
            fmt_out.i_visible_width = fmt_out.i_width;
            fmt_out.i_visible_height = fmt_out.i_height;
            p_converted = image_Convert( p_sys->p_image, p_es->p_picture,
                                         &fmt_in, &fmt_out );
            if( !p_converted )
            {
                msg_Warn( p_filter,
                           "image resizing and chroma conversion failed" );
                continue;
            }
        }
        else
        {
            p_converted = p_es->p_picture;
            fmt_in.i_width = fmt_out.i_width = p_converted->format.i_width;
            fmt_in.i_height = fmt_out.i_height = p_converted->format.i_height;
            fmt_in.i_chroma = fmt_out.i_chroma = p_converted->format.i_chroma;
            fmt_out.i_visible_width = fmt_out.i_width;
            fmt_out.i_visible_height = fmt_out.i_height;
        }
        p_region = subpicture_region_New( &fmt_out );
        /* FIXME the copy is probably not needed anymore */
        if( p_region )
            picture_Copy( p_region->p_picture, p_converted );
        if( !p_sys->b_keep )
            picture_Release( p_converted );
        if( !p_region )
        {
            msg_Err( p_filter, "cannot allocate SPU region" );
            p_filter->pf_sub_buffer_del( p_filter, p_spu );
            vlc_global_unlock( VLC_MOSAIC_MUTEX );
            vlc_mutex_unlock( &p_sys->lock );
            return NULL;
        }
        if( p_es->i_x >= 0 && p_es->i_y >= 0 )
        {
            p_region->i_x = p_es->i_x;
            p_region->i_y = p_es->i_y;
        }
        else if( p_sys->i_position == position_offsets )
        {
            p_region->i_x = p_sys->pi_x_offsets[i_real_index];
            p_region->i_y = p_sys->pi_y_offsets[i_real_index];
        }
        else
        {
            if( fmt_out.i_width > col_inner_width ||
                p_sys->b_ar || p_sys->b_keep )
            {
                /* we don't have to center the video since it takes the
                whole rectangle area or it's larger than the rectangle */
                p_region->i_x = p_sys->i_xoffset
                            + i_col * ( p_sys->i_width / p_sys->i_cols )
                            + ( i_col * p_sys->i_borderw ) / p_sys->i_cols;
            }
            else
            {
                /* center the video in the dedicated rectangle */
                p_region->i_x = p_sys->i_xoffset
                        + i_col * ( p_sys->i_width / p_sys->i_cols )
                        + ( i_col * p_sys->i_borderw ) / p_sys->i_cols
                        + ( col_inner_width - fmt_out.i_width ) / 2;
            }
            if( fmt_out.i_height > row_inner_height
                || p_sys->b_ar || p_sys->b_keep )
            {
                /* we don't have to center the video since it takes the
                whole rectangle area or it's taller than the rectangle */
                p_region->i_y = p_sys->i_yoffset
                        + i_row * ( p_sys->i_height / p_sys->i_rows )
                        + ( i_row * p_sys->i_borderh ) / p_sys->i_rows;
            }
            else
            {
                /* center the video in the dedicated rectangle */
                p_region->i_y = p_sys->i_yoffset
                        + i_row * ( p_sys->i_height / p_sys->i_rows )
                        + ( i_row * p_sys->i_borderh ) / p_sys->i_rows
                        + ( row_inner_height - fmt_out.i_height ) / 2;
            }
        }
        p_region->i_align = p_sys->i_align;
        p_region->i_alpha = p_es->i_alpha;
        if( p_region_prev == NULL )
        {
            p_spu->p_region = p_region;
        }
        else
        {
            p_region_prev->p_next = p_region;
        }
        p_region_prev = p_region;
    }
    vlc_global_unlock( VLC_MOSAIC_MUTEX );
    vlc_mutex_unlock( &p_sys->lock );
    return p_spu;
}

/* This is the filter function. See Open(). */
picture_t *Deinterlace( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_dst[DEINTERLACE_DST_SIZE];
    /* Request output picture */
    p_dst[0] = filter_NewPicture( p_filter );
    if( p_dst[0] == NULL )
    {
        picture_Release( p_pic );
        return NULL;
    }
    picture_CopyProperties( p_dst[0], p_pic );
    /* Any unused p_dst pointers must be NULL, because they are used to
       check how many output frames we have. */
    for( int i = 1; i < DEINTERLACE_DST_SIZE; ++i )
        p_dst[i] = NULL;
    /* Update the input frame history, if the currently active algorithm
       needs it. */
    if( p_sys->b_use_frame_history )
    {
        /* Duplicate the picture
         * TODO when the vout rework is finished, picture_Hold() might be enough
         * but becarefull, the pitches must match */
        picture_t *p_dup = picture_NewFromFormat( &p_pic->format );
        if( p_dup )
            picture_Copy( p_dup, p_pic );
        /* Slide the history */
        if( p_sys->pp_history[0] )
            picture_Release( p_sys->pp_history[0] );
        for( int i = 1; i < HISTORY_SIZE; i++ )
            p_sys->pp_history[i-1] = p_sys->pp_history[i];
        p_sys->pp_history[HISTORY_SIZE-1] = p_dup;
    }
    /* Slide the metadata history. */
    for( int i = 1; i < METADATA_SIZE; i++ )
    {
        p_sys->meta.pi_date[i-1]            = p_sys->meta.pi_date[i];
        p_sys->meta.pi_nb_fields[i-1]       = p_sys->meta.pi_nb_fields[i];
        p_sys->meta.pb_top_field_first[i-1] = p_sys->meta.pb_top_field_first[i];
    }
    /* The last element corresponds to the current input frame. */
    p_sys->meta.pi_date[METADATA_SIZE-1]            = p_pic->date;
    p_sys->meta.pi_nb_fields[METADATA_SIZE-1]       = p_pic->i_nb_fields;
    p_sys->meta.pb_top_field_first[METADATA_SIZE-1] = p_pic->b_top_field_first;
    /* Remember the frame offset that we should use for this frame.
       The value in p_sys will be updated to reflect the correct value
       for the *next* frame when we call the renderer. */
    int i_frame_offset = p_sys->i_frame_offset;
    int i_meta_idx     = (METADATA_SIZE-1) - i_frame_offset;
    /* These correspond to the current *outgoing* frame. */
    bool b_top_field_first;
    int i_nb_fields;
    if( i_frame_offset != CUSTOM_PTS )
    {
        /* Pick the correct values from the history. */
        b_top_field_first = p_sys->meta.pb_top_field_first[i_meta_idx];
        i_nb_fields       = p_sys->meta.pi_nb_fields[i_meta_idx];
    }
    else
    {
        /* Framerate doublers must not request CUSTOM_PTS, as they need the
           original field timings, and need Deinterlace() to allocate the
           correct number of output frames. */
        assert( !p_sys->b_double_rate );
        /* NOTE: i_nb_fields is only used for framerate doublers, so it is
                 unused in this case. b_top_field_first is only passed to the
                 algorithm. We assume that algorithms that request CUSTOM_PTS
                 will, if necessary, extract the TFF/BFF information themselves.
        */
        b_top_field_first = p_pic->b_top_field_first; /* this is not guaranteed
                                                         to be meaningful */
        i_nb_fields       = p_pic->i_nb_fields;       /* unused */
    }
    /* For framerate doublers, determine field duration and allocate
       output frames. */
    mtime_t i_field_dur = 0;
    int i_double_rate_alloc_end = 0; /* One past last for allocated output
                                        frames in p_dst[]. Used only for
                                        framerate doublers. Will be inited
                                        below. Declared here because the
                                        PTS logic needs the result. */
    if( p_sys->b_double_rate )
    {
        /* Calculate one field duration. */
        int i = 0;
        int iend = METADATA_SIZE-1;
        /* Find oldest valid logged date.
           The current input frame doesn't count. */
        for( ; i < iend; i++ )
            if( p_sys->meta.pi_date[i] > VLC_TS_INVALID )
                break;
        if( i < iend )
        {
            /* Count how many fields the valid history entries
               (except the new frame) represent. */
            int i_fields_total = 0;
            for( int j = i ; j < iend; j++ )
                i_fields_total += p_sys->meta.pi_nb_fields[j];
            /* One field took this long. */
            i_field_dur = (p_pic->date - p_sys->meta.pi_date[i]) / i_fields_total;
        }
        /* Note that we default to field duration 0 if it could not be
           determined. This behaves the same as the old code - leaving the
           extra output frame dates the same as p_pic->date if the last cached
           date was not valid.
        */
        i_double_rate_alloc_end = i_nb_fields;
        if( i_nb_fields > DEINTERLACE_DST_SIZE )
        {
            /* Note that the effective buffer size depends also on the constant
               private_picture in vout_wrapper.c, since that determines the
               maximum number of output pictures filter_NewPicture() will
               successfully allocate for one input frame.
            */
            msg_Err( p_filter, "Framerate doubler: output buffer too small; "\
                               "fields = %d, buffer size = %d. Dropping the "\
                               "remaining fields.",
                               i_nb_fields, DEINTERLACE_DST_SIZE );
            i_double_rate_alloc_end = DEINTERLACE_DST_SIZE;
        }
        /* Allocate output frames. */
        for( int i = 1; i < i_double_rate_alloc_end ; ++i )
        {
            p_dst[i-1]->p_next =
            p_dst[i]           = filter_NewPicture( p_filter );
            if( p_dst[i] )
            {
                picture_CopyProperties( p_dst[i], p_pic );
            }
            else
            {
                msg_Err( p_filter, "Framerate doubler: could not allocate "\
                                   "output frame %d", i+1 );
                i_double_rate_alloc_end = i; /* Inform the PTS logic about the
                                                correct end position. */
                break; /* If this happens, the rest of the allocations
                          aren't likely to work, either... */
            }
        }
        /* Now we have allocated *up to* the correct number of frames;
           normally, exactly the correct number. Upon alloc failure,
           we may have succeeded in allocating *some* output frames,
           but fewer than were desired. In such a case, as many will
           be rendered as were successfully allocated.
           Note that now p_dst[i] != NULL
           for 0 <= i < i_double_rate_alloc_end. */
    }
    assert( p_sys->b_double_rate  ||  p_dst[1] == NULL );
    assert( i_nb_fields > 2  ||  p_dst[2] == NULL );
    /* Render */
    switch( p_sys->i_mode )
    {
        case DEINTERLACE_DISCARD:
            RenderDiscard( p_dst[0], p_pic, 0 );
            break;
        case DEINTERLACE_BOB:
            RenderBob( p_dst[0], p_pic, !b_top_field_first );
            if( p_dst[1] )
                RenderBob( p_dst[1], p_pic, b_top_field_first );
            if( p_dst[2] )
                RenderBob( p_dst[2], p_pic, !b_top_field_first );
            break;;
        case DEINTERLACE_LINEAR:
            RenderLinear( p_filter, p_dst[0], p_pic, !b_top_field_first );
            if( p_dst[1] )
                RenderLinear( p_filter, p_dst[1], p_pic, b_top_field_first );
            if( p_dst[2] )
                RenderLinear( p_filter, p_dst[2], p_pic, !b_top_field_first );
            break;
        case DEINTERLACE_MEAN:
            RenderMean( p_filter, p_dst[0], p_pic );
            break;
        case DEINTERLACE_BLEND:
            RenderBlend( p_filter, p_dst[0], p_pic );
            break;
        case DEINTERLACE_X:
            RenderX( p_dst[0], p_pic );
            break;
        case DEINTERLACE_YADIF:
            if( RenderYadif( p_filter, p_dst[0], p_pic, 0, 0 ) )
                goto drop;
            break;
        case DEINTERLACE_YADIF2X:
            if( RenderYadif( p_filter, p_dst[0], p_pic, 0, !b_top_field_first ) )
                goto drop;
            if( p_dst[1] )
                RenderYadif( p_filter, p_dst[1], p_pic, 1, b_top_field_first );
            if( p_dst[2] )
                RenderYadif( p_filter, p_dst[2], p_pic, 2, !b_top_field_first );
            break;
        case DEINTERLACE_PHOSPHOR:
            if( RenderPhosphor( p_filter, p_dst[0], 0,
                                !b_top_field_first ) )
                goto drop;
            if( p_dst[1] )
                RenderPhosphor( p_filter, p_dst[1], 1,
                                b_top_field_first );
            if( p_dst[2] )
                RenderPhosphor( p_filter, p_dst[2], 2,
                                !b_top_field_first );
            break;
        case DEINTERLACE_IVTC:
            /* Note: RenderIVTC will automatically drop the duplicate frames
                     produced by IVTC. This is part of normal operation. */
            if( RenderIVTC( p_filter, p_dst[0] ) )
                goto drop;
            break;
    }
    /* Set output timestamps, if the algorithm didn't request CUSTOM_PTS
       for this frame. */
    assert( i_frame_offset <= METADATA_SIZE  ||  i_frame_offset == CUSTOM_PTS );
    if( i_frame_offset != CUSTOM_PTS )
    {
        mtime_t i_base_pts = p_sys->meta.pi_date[i_meta_idx];
        /* Note: in the usual case (i_frame_offset = 0  and
                 b_double_rate = false), this effectively does nothing.
                 This is needed to correct the timestamp
                 when i_frame_offset > 0. */
        p_dst[0]->date = i_base_pts;
        if( p_sys->b_double_rate )
        {
            /* Processing all actually allocated output frames. */
            for( int i = 1; i < i_double_rate_alloc_end; ++i )
            {
                /* XXX it's not really good especially for the first picture, but
                 * I don't think that delaying by one frame is worth it */
                if( i_base_pts > VLC_TS_INVALID )
                    p_dst[i]->date = i_base_pts + i * i_field_dur;
                else
                    p_dst[i]->date = VLC_TS_INVALID;
            }
        }
    }
    for( int i = 0; i < DEINTERLACE_DST_SIZE; ++i )
    {
        if( p_dst[i] )
        {
            p_dst[i]->b_progressive = true;
            p_dst[i]->i_nb_fields = 2;
        }
    }
    picture_Release( p_pic );
    return p_dst[0];
drop:
    picture_Release( p_dst[0] );
    for( int i = 1; i < DEINTERLACE_DST_SIZE; ++i )
    {
        if( p_dst[i] )
            picture_Release( p_dst[i] );
    }
    picture_Release( p_pic );
    return NULL;
}

 *****************************************************************************/
static int reinit_buffers( filter_t *p_filter )
{
    filter_sys_t *p = p_filter->p_sys;
    unsigned i,j;
    unsigned frames_stride = p->ms_stride * p->sample_rate / 1000.0;
    p->bytes_stride = frames_stride * p->bytes_per_frame;
    /* overlap */
    unsigned frames_overlap = frames_stride * p->percent_overlap;
    if( frames_overlap < 1 )
    { /* if no overlap */
        p->bytes_overlap    = 0;
        p->bytes_standing   = p->bytes_stride;
        p->samples_standing = p->bytes_standing / p->bytes_per_sample;
        p->output_overlap   = NULL;
    }
    else
    {
        unsigned prev_overlap   = p->bytes_overlap;
        p->bytes_overlap    = frames_overlap * p->bytes_per_frame;
        p->samples_overlap  = frames_overlap * p->samples_per_frame;
        p->bytes_standing   = p->bytes_stride - p->bytes_overlap;
        p->samples_standing = p->bytes_standing / p->bytes_per_sample;
        p->buf_overlap      = malloc( p->bytes_overlap );
        p->table_blend      = malloc( p->samples_overlap * 4 ); /* sizeof (int32|float) */
        if( !p->buf_overlap || !p->table_blend )
            return VLC_ENOMEM;
        if( p->bytes_overlap > prev_overlap )
            memset( (uint8_t *)p->buf_overlap + prev_overlap, 0, p->bytes_overlap - prev_overlap );
        float *pb = p->table_blend;
        float t = (float)frames_overlap;
        for( i = 0; i<frames_overlap; i++ )
        {
            float v = i / t;
            for( j = 0; j < p->samples_per_frame; j++ )
                *pb++ = v;
        }
        p->output_overlap = output_overlap_float;
    }
    /* best overlap */
    p->frames_search = ( frames_overlap <= 1 ) ? 0 : p->ms_search * p->sample_rate / 1000.0;
    if( p->frames_search < 1 )
    { /* if no search */
        p->best_overlap_offset = NULL;
    }
    else
    {
        unsigned bytes_pre_corr = ( p->samples_overlap - p->samples_per_frame ) * 4; /* sizeof (int32|float) */
        p->buf_pre_corr = malloc( bytes_pre_corr );
        p->table_window = malloc( bytes_pre_corr );
        if( ! p->buf_pre_corr || ! p->table_window )
            return VLC_ENOMEM;
        float *pw = p->table_window;
        for( i = 1; i<frames_overlap; i++ )
        {
            float v = i * ( frames_overlap - i );
            for( j = 0; j < p->samples_per_frame; j++ )
                *pw++ = v;
        }
        p->best_overlap_offset = best_overlap_offset_float;
    }
    unsigned new_size = ( p->frames_search + frames_stride + frames_overlap ) * p->bytes_per_frame;
    if( p->bytes_queued > new_size )
    {
        if( p->bytes_to_slide > p->bytes_queued )
        {
          p->bytes_to_slide -= p->bytes_queued;
          p->bytes_queued    = 0;
        }
        else
        {
            unsigned new_queued = __MIN( p->bytes_queued - p->bytes_to_slide, new_size );
            memmove( p->buf_queue,
                     p->buf_queue + p->bytes_queued - new_queued,
                     new_queued );
            p->bytes_to_slide = 0;
            p->bytes_queued   = new_queued;
        }
    }
    p->bytes_queue_max = new_size;
    p->buf_queue = malloc( p->bytes_queue_max );
    if( ! p->buf_queue )
        return VLC_ENOMEM;
    p->bytes_stride_scaled  = p->bytes_stride * p->scale;
    p->frames_stride_scaled = p->bytes_stride_scaled / p->bytes_per_frame;
    msg_Dbg( VLC_OBJECT(p_filter),
             "%.3f scale, %.3f stride_in, %i stride_out, %i standing, %i overlap, %i search, %i queue, %s mode",
             p->scale,
             p->frames_stride_scaled,
             (int)( p->bytes_stride / p->bytes_per_frame ),
             (int)( p->bytes_standing / p->bytes_per_frame ),
             (int)( p->bytes_overlap / p->bytes_per_frame ),
             p->frames_search,
             (int)( p->bytes_queue_max / p->bytes_per_frame ),
             "fl32");
    return VLC_SUCCESS;
}

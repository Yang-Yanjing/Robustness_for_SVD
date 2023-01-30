/* FrameSpanGetTime returns the time span represented by the frame span. */
static inline mtime_t FrameSpanGetTime( frame_span_t *span, uint32_t i_sample_rate )
{
    if( !i_sample_rate )
        return 0;
    return ( span->i_samples * CLOCK_FREQ ) / i_sample_rate + 1;
}

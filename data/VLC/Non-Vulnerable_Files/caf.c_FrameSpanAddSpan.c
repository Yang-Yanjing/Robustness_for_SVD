/* FrameSpanAddSpan adds span2 to span1 */
static inline void FrameSpanAddSpan( frame_span_t *span1, frame_span_t *span2 )
{
    span1->i_frames += span2->i_frames;
    span1->i_samples += span2->i_samples;
    span1->i_bytes += span2->i_bytes;
    span1->i_desc_bytes += span2->i_desc_bytes;
}

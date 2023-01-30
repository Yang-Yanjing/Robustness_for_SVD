}
static block_t *vlc_av_packet_Wrap(AVPacket *packet, mtime_t i_length, AVCodecContext *context )
{
    vlc_av_packet_t *b = malloc( sizeof( *b ) );
    if( unlikely(b == NULL) )
        return NULL;
    block_t *p_block = &b->self;
    block_Init(p_block, packet->data, packet->size);
    p_block->i_nb_samples = 0;
    p_block->pf_release = vlc_av_packet_Release;
    b->packet = *packet;
    p_block->i_length = i_length;
    p_block->i_pts = packet->pts;
    p_block->i_dts = packet->dts;
    if( unlikely( packet->flags & AV_PKT_FLAG_CORRUPT ) )
        p_block->i_flags |= BLOCK_FLAG_CORRUPTED;
    p_block->i_pts = p_block->i_pts * CLOCK_FREQ * context->time_base.num / context->time_base.den;
    p_block->i_dts = p_block->i_dts * CLOCK_FREQ * context->time_base.num / context->time_base.den;
    return p_block;
}

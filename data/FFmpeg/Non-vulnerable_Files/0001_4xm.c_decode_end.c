static av_cold int decode_end(AVCodecContext *avctx)
{
    FourXContext * const f = avctx->priv_data;
    int i;
    av_freep(&f->frame_buffer);
    av_freep(&f->last_frame_buffer);
    av_freep(&f->bitstream_buffer);
    f->bitstream_buffer_size = 0;
    for (i = 0; i < CFRAME_BUFFER_COUNT; i++) {
        av_freep(&f->cfrm[i].data);
        f->cfrm[i].allocated_size = 0;
    }
    ff_free_vlc(&f->pre_vlc);
    return 0;
}
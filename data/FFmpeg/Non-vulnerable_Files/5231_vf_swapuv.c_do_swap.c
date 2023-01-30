static void do_swap(AVFrame *frame)
{
    FFSWAP(uint8_t*,     frame->data[1],     frame->data[2]);
    FFSWAP(int,          frame->linesize[1], frame->linesize[2]);
    FFSWAP(uint64_t,     frame->error[1],    frame->error[2]);
    FFSWAP(AVBufferRef*, frame->buf[1],      frame->buf[2]);
}

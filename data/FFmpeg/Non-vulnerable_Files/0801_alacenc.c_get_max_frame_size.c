static av_always_inline int get_max_frame_size(int frame_size, int ch, int bps)
{
    int header_bits = 23 + 32 * (frame_size < DEFAULT_FRAME_SIZE);
    return FFALIGN(header_bits + bps * ch * frame_size + 3, 8) / 8;
}

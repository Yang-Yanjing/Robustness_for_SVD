static inline void draw_dot(AVFrame *out, int x, int y, uint8_t fg[4])
{
    uint32_t color = AV_RL32(out->data[0] + y * out->linesize[0] + x * 4);
    if ((color & 0xffffff) != 0)
        AV_WL32(out->data[0] + y * out->linesize[0] + x * 4, AV_RL32(fg) | color);
    else
        AV_WL32(out->data[0] + y * out->linesize[0] + x * 4, AV_RL32(fg));
}

static inline void draw_dot(int fg, int x, int y, AVFrame *out)
{
    AV_WN32(out->data[0] + y * out->linesize[0] + x * 4, fg);
}

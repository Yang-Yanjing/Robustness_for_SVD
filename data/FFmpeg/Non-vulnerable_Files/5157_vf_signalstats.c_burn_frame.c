static void burn_frame(const SignalstatsContext *s, AVFrame *f, int x, int y)
{
    const int chromax = x >> s->hsub;
    const int chromay = y >> s->vsub;
    f->data[0][y       * f->linesize[0] +       x] = s->yuv_color[0];
    f->data[1][chromay * f->linesize[1] + chromax] = s->yuv_color[1];
    f->data[2][chromay * f->linesize[2] + chromax] = s->yuv_color[2];
}

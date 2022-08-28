static void clear_image(DrawGraphContext *s, AVFrame *out, AVFilterLink *outlink)
{
    int i, j;
    int bg = AV_RN32(s->bg);
    for (i = 0; i < out->height; i++)
        for (j = 0; j < out->width; j++)
            AV_WN32(out->data[0] + i * out->linesize[0] + j * 4, bg);
}

static int update_histogram_diff(struct hist_node *hist,
                                 const AVFrame *f1, const AVFrame *f2)
{
    int x, y, ret, nb_diff_colors = 0;
    for (y = 0; y < f1->height; y++) {
        const uint32_t *p = (const uint32_t *)(f1->data[0] + y*f1->linesize[0]);
        const uint32_t *q = (const uint32_t *)(f2->data[0] + y*f2->linesize[0]);
        for (x = 0; x < f1->width; x++) {
            if (p[x] == q[x])
                continue;
            ret = color_inc(hist, p[x]);
            if (ret < 0)
                return ret;
            nb_diff_colors += ret;
        }
    }
    return nb_diff_colors;
}

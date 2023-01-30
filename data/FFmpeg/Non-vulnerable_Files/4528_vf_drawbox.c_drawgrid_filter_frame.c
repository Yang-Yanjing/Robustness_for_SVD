static int drawgrid_filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    DrawBoxContext *drawgrid = inlink->dst->priv;
    int plane, x, y;
    uint8_t *row[4];
    for (y = 0; y < frame->height; y++) {
        row[0] = frame->data[0] + y * frame->linesize[0];
        for (plane = 1; plane < 3; plane++)
            row[plane] = frame->data[plane] +
                 frame->linesize[plane] * (y >> drawgrid->vsub);
        if (drawgrid->invert_color) {
            for (x = 0; x < frame->width; x++)
                if (pixel_belongs_to_grid(drawgrid, x, y))
                    row[0][x] = 0xff - row[0][x];
        } else {
            for (x = 0; x < frame->width; x++) {
                double alpha = (double)drawgrid->yuv_color[A] / 255;
                if (pixel_belongs_to_grid(drawgrid, x, y)) {
                    row[0][x                  ] = (1 - alpha) * row[0][x                  ] + alpha * drawgrid->yuv_color[Y];
                    row[1][x >> drawgrid->hsub] = (1 - alpha) * row[1][x >> drawgrid->hsub] + alpha * drawgrid->yuv_color[U];
                    row[2][x >> drawgrid->hsub] = (1 - alpha) * row[2][x >> drawgrid->hsub] + alpha * drawgrid->yuv_color[V];
                }
            }
        }
    }
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}

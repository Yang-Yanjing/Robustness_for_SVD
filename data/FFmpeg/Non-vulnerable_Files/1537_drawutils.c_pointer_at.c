static uint8_t *pointer_at(FFDrawContext *draw, uint8_t *data[], int linesize[],
                           int plane, int x, int y)
{
    return data[plane] +
           (y >> draw->vsub[plane]) * linesize[plane] +
           (x >> draw->hsub[plane]) * draw->pixelstep[plane];
}

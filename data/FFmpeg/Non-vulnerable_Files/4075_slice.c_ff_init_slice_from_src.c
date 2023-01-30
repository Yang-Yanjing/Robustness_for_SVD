int ff_init_slice_from_src(SwsSlice * s, uint8_t *src[4], int stride[4], int srcW, int lumY, int lumH, int chrY, int chrH)
{
    int i = 0;
    const int start[4] = {lumY,
                    chrY,
                    chrY,
                    lumY};
    const int end[4] = {lumY +lumH,
                        chrY + chrH,
                        chrY + chrH,
                        lumY + lumH};
    s->width = srcW;
    for (i = 0; i < 4; ++i) {
        int j;
        int lines = end[i];
        lines = s->plane[i].available_lines < lines ? s->plane[i].available_lines : lines;
        if (end[i] > s->plane[i].sliceY+s->plane[i].sliceH) {
            if (start[i] <= s->plane[i].sliceY+1)
                s->plane[i].sliceY = FFMIN(start[i], s->plane[i].sliceY);
            else
                s->plane[i].sliceY = start[i];
            s->plane[i].sliceH = end[i] - s->plane[i].sliceY;
        } else {
            if (end[i] >= s->plane[i].sliceY)
                s->plane[i].sliceH = s->plane[i].sliceY + s->plane[i].sliceH - start[i];
            else
                s->plane[i].sliceH = end[i] - start[i];
            s->plane[i].sliceY = start[i];
        }
        for (j = start[i]; j < lines; j+= 1)
            s->plane[i].line[j] = src[i] + (start[i] + j) * stride[i];
    }
    return 0;
}

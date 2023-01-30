static void fade(AudioVectorScopeContext *s)
{
    const int linesize = s->outpicref->linesize[0];
    int i, j;
    if (s->fade[0] || s->fade[1] || s->fade[2]) {
        uint8_t *d = s->outpicref->data[0];
        for (i = 0; i < s->h; i++) {
            for (j = 0; j < s->w*4; j+=4) {
                d[j+0] = FFMAX(d[j+0] - s->fade[0], 0);
                d[j+1] = FFMAX(d[j+1] - s->fade[1], 0);
                d[j+2] = FFMAX(d[j+2] - s->fade[2], 0);
                d[j+3] = FFMAX(d[j+3] - s->fade[3], 0);
            }
            d += linesize;
        }
    }
}

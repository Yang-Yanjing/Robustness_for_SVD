static int request_frame(AVFilterLink *outlink)
{
    MPTestContext *test = outlink->src->priv;
    AVFrame *picref;
    int w = WIDTH, h = HEIGHT,
        cw = FF_CEIL_RSHIFT(w, test->hsub), ch = FF_CEIL_RSHIFT(h, test->vsub);
    unsigned int frame = outlink->frame_count;
    enum test_type tt = test->test;
    int i;
    if (test->max_pts >= 0 && test->pts > test->max_pts)
        return AVERROR_EOF;
    picref = ff_get_video_buffer(outlink, w, h);
    if (!picref)
        return AVERROR(ENOMEM);
    picref->pts = test->pts++;
    
    for (i = 0; i < h; i++)
        memset(picref->data[0] + i*picref->linesize[0], 0, w);
    for (i = 0; i < ch; i++) {
        memset(picref->data[1] + i*picref->linesize[1], 128, cw);
        memset(picref->data[2] + i*picref->linesize[2], 128, cw);
    }
    if (tt == TEST_ALL && frame%30) 
        tt = (frame/30)%(TEST_NB-1);
    switch (tt) {
    case TEST_DC_LUMA:       dc_test(picref->data[0], picref->linesize[0], 256, 256, frame%30); break;
    case TEST_DC_CHROMA:     dc_test(picref->data[1], picref->linesize[1], 256, 256, frame%30); break;
    case TEST_FREQ_LUMA:   freq_test(picref->data[0], picref->linesize[0], frame%30); break;
    case TEST_FREQ_CHROMA: freq_test(picref->data[1], picref->linesize[1], frame%30); break;
    case TEST_AMP_LUMA:     amp_test(picref->data[0], picref->linesize[0], frame%30); break;
    case TEST_AMP_CHROMA:   amp_test(picref->data[1], picref->linesize[1], frame%30); break;
    case TEST_CBP:          cbp_test(picref->data   , picref->linesize   , frame%30); break;
    case TEST_MV:            mv_test(picref->data[0], picref->linesize[0], frame%30); break;
    case TEST_RING1:      ring1_test(picref->data[0], picref->linesize[0], frame%30); break;
    case TEST_RING2:      ring2_test(picref->data[0], picref->linesize[0], frame%30); break;
    }
    return ff_filter_frame(outlink, picref);
}

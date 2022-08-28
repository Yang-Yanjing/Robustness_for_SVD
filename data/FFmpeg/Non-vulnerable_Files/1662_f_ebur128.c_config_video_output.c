static int config_video_output(AVFilterLink *outlink)
{
    int i, x, y;
    uint8_t *p;
    AVFilterContext *ctx = outlink->src;
    EBUR128Context *ebur128 = ctx->priv;
    AVFrame *outpicref;
    
    if (ebur128->w < 640 || ebur128->h < 480) {
        av_log(ctx, AV_LOG_ERROR, "Video size %dx%d is too small, "
               "minimum size is 640x480\n", ebur128->w, ebur128->h);
        return AVERROR(EINVAL);
    }
    outlink->w = ebur128->w;
    outlink->h = ebur128->h;
#define PAD 8
    
    ebur128->text.x  = PAD;
    ebur128->text.y  = 40;
    ebur128->text.w  = 3 * 8;   
    ebur128->text.h  = ebur128->h - PAD - ebur128->text.y;
    
    ebur128->gauge.w = 20;
    ebur128->gauge.h = ebur128->text.h;
    ebur128->gauge.x = ebur128->w - PAD - ebur128->gauge.w;
    ebur128->gauge.y = ebur128->text.y;
    
    ebur128->graph.x = ebur128->text.x + ebur128->text.w + PAD;
    ebur128->graph.y = ebur128->gauge.y;
    ebur128->graph.w = ebur128->gauge.x - ebur128->graph.x - PAD;
    ebur128->graph.h = ebur128->gauge.h;
    
    av_assert0(ebur128->graph.h == ebur128->gauge.h);
    
    av_frame_free(&ebur128->outpicref);
    ebur128->outpicref = outpicref =
        ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!outpicref)
        return AVERROR(ENOMEM);
    outlink->sample_aspect_ratio = (AVRational){1,1};
    
    ebur128->y_line_ref = av_calloc(ebur128->graph.h + 1, sizeof(*ebur128->y_line_ref));
    if (!ebur128->y_line_ref)
        return AVERROR(ENOMEM);
    
    memset(outpicref->data[0], 0, ebur128->h * outpicref->linesize[0]);
    
    drawtext(outpicref, PAD, PAD+16, FONT8, font_colors+3, " LU");
    for (i = ebur128->meter; i >= -ebur128->meter * 2; i--) {
        y = lu_to_y(ebur128, i);
        x = PAD + (i < 10 && i > -10) * 8;
        ebur128->y_line_ref[y] = i;
        y -= 4; 
        drawtext(outpicref, x, y + ebur128->graph.y, FONT8, font_colors+3,
                 "%c%d", i < 0 ? '-' : i > 0 ? '+' : ' ', FFABS(i));
    }
    
    ebur128->y_zero_lu = lu_to_y(ebur128, 0);
    p = outpicref->data[0] + ebur128->graph.y * outpicref->linesize[0]
                           + ebur128->graph.x * 3;
    for (y = 0; y < ebur128->graph.h; y++) {
        const uint8_t *c = get_graph_color(ebur128, INT_MAX, y);
        for (x = 0; x < ebur128->graph.w; x++)
            memcpy(p + x*3, c, 3);
        p += outpicref->linesize[0];
    }
    
#define DRAW_RECT(r) do { \
    drawline(outpicref, r.x,       r.y - 1,   r.w, 3); \
    drawline(outpicref, r.x,       r.y + r.h, r.w, 3); \
    drawline(outpicref, r.x - 1,   r.y,       r.h, outpicref->linesize[0]); \
    drawline(outpicref, r.x + r.w, r.y,       r.h, outpicref->linesize[0]); \
} while (0)
    DRAW_RECT(ebur128->graph);
    DRAW_RECT(ebur128->gauge);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}

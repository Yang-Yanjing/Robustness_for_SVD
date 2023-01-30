static int movie_request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    unsigned out_id = FF_OUTLINK_IDX(outlink);
    int ret;
    while (1) {
        ret = movie_push_frame(ctx, out_id);
        if (ret)
            return FFMIN(ret, 0);
    }
}

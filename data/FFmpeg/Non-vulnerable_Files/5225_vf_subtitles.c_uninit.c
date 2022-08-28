static av_cold void uninit(AVFilterContext *ctx)
{
    AssContext *ass = ctx->priv;
    if (ass->track)
        ass_free_track(ass->track);
    if (ass->renderer)
        ass_renderer_done(ass->renderer);
    if (ass->library)
        ass_library_done(ass->library);
}

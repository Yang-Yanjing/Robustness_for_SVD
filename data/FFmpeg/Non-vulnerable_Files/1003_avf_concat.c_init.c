static av_cold int init(AVFilterContext *ctx)
{
    ConcatContext *cat = ctx->priv;
    unsigned seg, type, str;
    
    for (seg = 0; seg < cat->nb_segments; seg++) {
        for (type = 0; type < TYPE_ALL; type++) {
            for (str = 0; str < cat->nb_streams[type]; str++) {
                AVFilterPad pad = {
                    .type             = type,
                    .get_video_buffer = get_video_buffer,
                    .get_audio_buffer = get_audio_buffer,
                    .filter_frame     = filter_frame,
                };
                pad.name = av_asprintf("in%d:%c%d", seg, "va"[type], str);
                ff_insert_inpad(ctx, ctx->nb_inputs, &pad);
            }
        }
    }
    
    for (type = 0; type < TYPE_ALL; type++) {
        for (str = 0; str < cat->nb_streams[type]; str++) {
            AVFilterPad pad = {
                .type          = type,
                .config_props  = config_output,
                .request_frame = request_frame,
            };
            pad.name = av_asprintf("out:%c%d", "va"[type], str);
            ff_insert_outpad(ctx, ctx->nb_outputs, &pad);
        }
    }
    cat->in = av_calloc(ctx->nb_inputs, sizeof(*cat->in));
    if (!cat->in)
        return AVERROR(ENOMEM);
    cat->nb_in_active = ctx->nb_outputs;
    return 0;
}

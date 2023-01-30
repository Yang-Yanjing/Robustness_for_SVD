static av_cold void uninit(AVFilterContext *ctx)
{
    SSIMContext *s = ctx->priv;
    if (s->nb_frames > 0) {
        char buf[256];
        int i;
        buf[0] = 0;
        for (i = 0; i < s->nb_components; i++) {
            int c = s->is_rgb ? s->rgba_map[i] : i;
            av_strlcatf(buf, sizeof(buf), " %c:%f", s->comps[i], s->ssim[c] / s->nb_frames);
        }
        av_log(ctx, AV_LOG_INFO, "SSIM%s All:%f (%f)\n", buf,
               s->ssim_total / s->nb_frames, ssim_db(s->ssim_total, s->nb_frames));
    }
    ff_dualinput_uninit(&s->dinput);
    if (s->stats_file)
        fclose(s->stats_file);
    av_freep(&s->temp);
}

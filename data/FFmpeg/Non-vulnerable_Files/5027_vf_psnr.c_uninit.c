static av_cold void uninit(AVFilterContext *ctx)
{
    PSNRContext *s = ctx->priv;
    if (s->nb_frames > 0) {
        int j;
        char buf[256];
        buf[0] = 0;
        for (j = 0; j < s->nb_components; j++) {
            int c = s->is_rgb ? s->rgba_map[j] : j;
            av_strlcatf(buf, sizeof(buf), " %c:%0.2f", s->comps[j],
                        get_psnr(s->mse_comp[c], s->nb_frames, s->max[c]));
        }
        av_log(ctx, AV_LOG_INFO, "PSNR%s average:%0.2f min:%0.2f max:%0.2f\n",
               buf,
               get_psnr(s->mse, s->nb_frames, s->average_max),
               get_psnr(s->max_mse, 1, s->average_max),
               get_psnr(s->min_mse, 1, s->average_max));
    }
    ff_dualinput_uninit(&s->dinput);
    if (s->stats_file)
        fclose(s->stats_file);
}

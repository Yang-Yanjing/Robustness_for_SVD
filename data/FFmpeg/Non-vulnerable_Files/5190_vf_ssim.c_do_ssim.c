static AVFrame *do_ssim(AVFilterContext *ctx, AVFrame *main,
                        const AVFrame *ref)
{
    AVDictionary **metadata = avpriv_frame_get_metadatap(main);
    SSIMContext *s = ctx->priv;
    float c[4], ssimv = 0.0;
    int i;
    s->nb_frames++;
    for (i = 0; i < s->nb_components; i++) {
        c[i] = ssim_plane(&s->dsp, main->data[i], main->linesize[i],
                          ref->data[i], ref->linesize[i],
                          s->planewidth[i], s->planeheight[i], s->temp);
        ssimv += s->coefs[i] * c[i];
        s->ssim[i] += c[i];
    }
    for (i = 0; i < s->nb_components; i++) {
        int cidx = s->is_rgb ? s->rgba_map[i] : i;
        set_meta(metadata, "lavfi.ssim.", s->comps[i], c[cidx]);
    }
    s->ssim_total += ssimv;
    set_meta(metadata, "lavfi.ssim.All", 0, ssimv);
    set_meta(metadata, "lavfi.ssim.dB", 0, ssim_db(ssimv, 1.0));
    if (s->stats_file) {
        fprintf(s->stats_file, "n:%"PRId64" ", s->nb_frames);
        for (i = 0; i < s->nb_components; i++) {
            int cidx = s->is_rgb ? s->rgba_map[i] : i;
            fprintf(s->stats_file, "%c:%f ", s->comps[i], c[cidx]);
        }
        fprintf(s->stats_file, "All:%f (%f)\n", ssimv, ssim_db(ssimv, 1.0));
    }
    return main;
}

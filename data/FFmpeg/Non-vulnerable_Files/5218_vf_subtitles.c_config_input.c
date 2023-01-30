static int config_input(AVFilterLink *inlink)
{
    AssContext *ass = inlink->dst->priv;
    ff_draw_init(&ass->draw, inlink->format, 0);
    ass_set_frame_size  (ass->renderer, inlink->w, inlink->h);
    if (ass->original_w && ass->original_h)
        ass_set_aspect_ratio(ass->renderer, (double)inlink->w / inlink->h,
                             (double)ass->original_w / ass->original_h);
    if (ass->shaping != -1)
        ass_set_shaper(ass->renderer, ass->shaping);
    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *inpic)
{
    HueContext *hue = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *outpic;
    const int32_t old_hue_sin = hue->hue_sin, old_hue_cos = hue->hue_cos;
    const float old_brightness = hue->brightness;
    int direct = 0;
    if (av_frame_is_writable(inpic)) {
        direct = 1;
        outpic = inpic;
    } else {
        outpic = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!outpic) {
            av_frame_free(&inpic);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(outpic, inpic);
    }
    hue->var_values[VAR_N]   = inlink->frame_count;
    hue->var_values[VAR_T]   = TS2T(inpic->pts, inlink->time_base);
    hue->var_values[VAR_PTS] = TS2D(inpic->pts);
    if (hue->saturation_expr) {
        hue->saturation = av_expr_eval(hue->saturation_pexpr, hue->var_values, NULL);
        if (hue->saturation < SAT_MIN_VAL || hue->saturation > SAT_MAX_VAL) {
            hue->saturation = av_clip(hue->saturation, SAT_MIN_VAL, SAT_MAX_VAL);
            av_log(inlink->dst, AV_LOG_WARNING,
                   "Saturation value not in range [%d,%d]: clipping value to %0.1f\n",
                   SAT_MIN_VAL, SAT_MAX_VAL, hue->saturation);
        }
    }
    if (hue->brightness_expr) {
        hue->brightness = av_expr_eval(hue->brightness_pexpr, hue->var_values, NULL);
        if (hue->brightness < -10 || hue->brightness > 10) {
            hue->brightness = av_clipf(hue->brightness, -10, 10);
            av_log(inlink->dst, AV_LOG_WARNING,
                   "Brightness value not in range [%d,%d]: clipping value to %0.1f\n",
                   -10, 10, hue->brightness);
        }
    }
    if (hue->hue_deg_expr) {
        hue->hue_deg = av_expr_eval(hue->hue_deg_pexpr, hue->var_values, NULL);
        hue->hue = hue->hue_deg * M_PI / 180;
    } else if (hue->hue_expr) {
        hue->hue = av_expr_eval(hue->hue_pexpr, hue->var_values, NULL);
        hue->hue_deg = hue->hue * 180 / M_PI;
    }
    av_log(inlink->dst, AV_LOG_DEBUG,
           "H:%0.1f*PI h:%0.1f s:%0.1f b:%0.f t:%0.1f n:%d\n",
           hue->hue/M_PI, hue->hue_deg, hue->saturation, hue->brightness,
           hue->var_values[VAR_T], (int)hue->var_values[VAR_N]);
    compute_sin_and_cos(hue);
    if (hue->is_first || (old_hue_sin != hue->hue_sin || old_hue_cos != hue->hue_cos))
        create_chrominance_lut(hue, hue->hue_cos, hue->hue_sin);
    if (hue->is_first || (old_brightness != hue->brightness && hue->brightness))
        create_luma_lut(hue);
    if (!direct) {
        if (!hue->brightness)
            av_image_copy_plane(outpic->data[0], outpic->linesize[0],
                                inpic->data[0],  inpic->linesize[0],
                                inlink->w, inlink->h);
        if (inpic->data[3])
            av_image_copy_plane(outpic->data[3], outpic->linesize[3],
                                inpic->data[3],  inpic->linesize[3],
                                inlink->w, inlink->h);
    }
    apply_lut(hue, outpic->data[1], outpic->data[2], outpic->linesize[1],
              inpic->data[1],  inpic->data[2],  inpic->linesize[1],
              FF_CEIL_RSHIFT(inlink->w, hue->hsub),
              FF_CEIL_RSHIFT(inlink->h, hue->vsub));
    if (hue->brightness)
        apply_luma_lut(hue, outpic->data[0], outpic->linesize[0],
                       inpic->data[0], inpic->linesize[0], inlink->w, inlink->h);
    if (!direct)
        av_frame_free(&inpic);
    hue->is_first = 0;
    return ff_filter_frame(outlink, outpic);
}

static int filter_frame(AVFilterLink *link, AVFrame *picref)
{
    AVFilterContext *ctx = link->dst;
    IDETContext *idet = ctx->priv;
    
    
    if (idet->analyze_interlaced_flag &&
        !picref->interlaced_frame &&
        !idet->next) {
        return ff_filter_frame(ctx->outputs[0], picref);
    }
    if (idet->analyze_interlaced_flag_done) {
        if (picref->interlaced_frame && idet->interlaced_flag_accuracy < 0)
            picref->interlaced_frame = 0;
        return ff_filter_frame(ctx->outputs[0], picref);
    }
    av_frame_free(&idet->prev);
    if(   picref->width  != link->w
       || picref->height != link->h
       || picref->format != link->format) {
        link->dst->inputs[0]->format = picref->format;
        link->dst->inputs[0]->w      = picref->width;
        link->dst->inputs[0]->h      = picref->height;
        av_frame_free(&idet->cur );
        av_frame_free(&idet->next);
    }
    idet->prev = idet->cur;
    idet->cur  = idet->next;
    idet->next = picref;
    if (!idet->cur &&
        !(idet->cur = av_frame_clone(idet->next)))
        return AVERROR(ENOMEM);
    if (!idet->prev)
        return 0;
    if (!idet->csp)
        idet->csp = av_pix_fmt_desc_get(link->format);
    if (idet->csp->comp[0].depth_minus1 / 8 == 1){
        idet->filter_line = (ff_idet_filter_func)ff_idet_filter_line_c_16bit;
        if (ARCH_X86)
            ff_idet_init_x86(idet, 1);
    }
    if (idet->analyze_interlaced_flag) {
        if (idet->cur->interlaced_frame) {
            idet->cur->interlaced_frame = 0;
            filter(ctx);
            if (idet->last_type == PROGRESSIVE) {
                idet->interlaced_flag_accuracy --;
                idet->analyze_interlaced_flag --;
            } else if (idet->last_type != UNDETERMINED) {
                idet->interlaced_flag_accuracy ++;
                idet->analyze_interlaced_flag --;
            }
            if (idet->analyze_interlaced_flag == 1) {
                ff_filter_frame(ctx->outputs[0], av_frame_clone(idet->cur));
                if (idet->next->interlaced_frame && idet->interlaced_flag_accuracy < 0)
                    idet->next->interlaced_frame = 0;
                idet->analyze_interlaced_flag_done = 1;
                av_log(ctx, AV_LOG_INFO, "Final flag accuracy %d\n", idet->interlaced_flag_accuracy);
                return ff_filter_frame(ctx->outputs[0], av_frame_clone(idet->next));
            }
        }
    } else {
        filter(ctx);
    }
    return ff_filter_frame(ctx->outputs[0], av_frame_clone(idet->cur));
}

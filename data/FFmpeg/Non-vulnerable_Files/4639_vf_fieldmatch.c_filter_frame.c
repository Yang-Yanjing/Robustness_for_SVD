static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    FieldMatchContext *fm = ctx->priv;
    int combs[] = { -1, -1, -1, -1, -1 };
    int order, field, i, match, sc = 0;
    const int *fxo;
    AVFrame *gen_frames[] = { NULL, NULL, NULL, NULL, NULL };
    AVFrame *dst;
    
#define SLIDING_FRAME_WINDOW(prv, src, nxt) do {                \
        if (prv != src)  \
            av_frame_free(&prv);                                \
        prv = src;                                              \
        src = nxt;                                              \
        if (in)                                                 \
            nxt = in;                                           \
        if (!prv)                                               \
            prv = src;                                          \
        if (!prv)    \
            return 0;                                           \
        av_assert0(prv && src && nxt);                          \
} while (0)
    if (FF_INLINK_IDX(inlink) == INPUT_MAIN) {
        SLIDING_FRAME_WINDOW(fm->prv, fm->src, fm->nxt);
        fm->got_frame[INPUT_MAIN] = 1;
    } else {
        SLIDING_FRAME_WINDOW(fm->prv2, fm->src2, fm->nxt2);
        fm->got_frame[INPUT_CLEANSRC] = 1;
    }
    if (!fm->got_frame[INPUT_MAIN] || (fm->ppsrc && !fm->got_frame[INPUT_CLEANSRC]))
        return 0;
    fm->got_frame[INPUT_MAIN] = fm->got_frame[INPUT_CLEANSRC] = 0;
    in = fm->src;
    
    order = fm->order != FM_PARITY_AUTO ? fm->order : (in->interlaced_frame ? in->top_field_first : 1);
    field = fm->field != FM_PARITY_AUTO ? fm->field : order;
    av_assert0(order == 0 || order == 1 || field == 0 || field == 1);
    fxo = field ^ order ? fxo1m : fxo0m;
    

    if (fm->combdbg) {
        for (i = 0; i < FF_ARRAY_ELEMS(combs); i++) {
            if (i > mN && fm->combdbg == COMBDBG_PCN)
                break;
            gen_frames[i] = create_weave_frame(ctx, i, field, fm->prv, fm->src, fm->nxt);
            if (!gen_frames[i])
                return AVERROR(ENOMEM);
            combs[i] = calc_combed_score(fm, gen_frames[i]);
        }
        av_log(ctx, AV_LOG_INFO, "COMBS: %3d %3d %3d %3d %3d\n",
               combs[0], combs[1], combs[2], combs[3], combs[4]);
    } else {
        gen_frames[mC] = av_frame_clone(fm->src);
        if (!gen_frames[mC])
            return AVERROR(ENOMEM);
    }
    
    match = compare_fields(fm, fxo[mC], fxo[mP], field);
    if (fm->mode == MODE_PCN || fm->mode == MODE_PCN_UB)
        match = compare_fields(fm, match, fxo[mN], field);
    
    if (fm->combmatch == COMBMATCH_SC) {
        if (fm->lastn == outlink->frame_count - 1) {
            if (fm->lastscdiff > fm->scthresh)
                sc = 1;
        } else if (luma_abs_diff(fm->prv, fm->src) > fm->scthresh) {
            sc = 1;
        }
        if (!sc) {
            fm->lastn = outlink->frame_count;
            fm->lastscdiff = luma_abs_diff(fm->src, fm->nxt);
            sc = fm->lastscdiff > fm->scthresh;
        }
    }
    if (fm->combmatch == COMBMATCH_FULL || (fm->combmatch == COMBMATCH_SC && sc)) {
        switch (fm->mode) {
        
        case MODE_PC:
            match = checkmm(ctx, combs, match, match == fxo[mP] ? fxo[mC] : fxo[mP], gen_frames, field);
            break;
        case MODE_PC_N:
            match = checkmm(ctx, combs, match, fxo[mN], gen_frames, field);
            break;
        case MODE_PC_U:
            match = checkmm(ctx, combs, match, fxo[mU], gen_frames, field);
            break;
        case MODE_PC_N_UB:
            match = checkmm(ctx, combs, match, fxo[mN], gen_frames, field);
            match = checkmm(ctx, combs, match, fxo[mU], gen_frames, field);
            match = checkmm(ctx, combs, match, fxo[mB], gen_frames, field);
            break;
        
        case MODE_PCN:
            match = checkmm(ctx, combs, match, match == fxo[mP] ? fxo[mC] : fxo[mP], gen_frames, field);
            break;
        case MODE_PCN_UB:
            match = checkmm(ctx, combs, match, fxo[mU], gen_frames, field);
            match = checkmm(ctx, combs, match, fxo[mB], gen_frames, field);
            break;
        default:
            av_assert0(0);
        }
    }
    
    if (fm->ppsrc) {
        

        dst = create_weave_frame(ctx, match, field, fm->prv2, fm->src2, fm->nxt2);
    } else {
        if (!gen_frames[match]) { 
            dst = create_weave_frame(ctx, match, field, fm->prv, fm->src, fm->nxt);
        } else {
            dst = gen_frames[match];
            gen_frames[match] = NULL;
        }
    }
    if (!dst)
        return AVERROR(ENOMEM);
    for (i = 0; i < FF_ARRAY_ELEMS(gen_frames); i++)
        av_frame_free(&gen_frames[i]);
    

    dst->interlaced_frame = combs[match] >= fm->combpel;
    if (dst->interlaced_frame) {
        av_log(ctx, AV_LOG_WARNING, "Frame #%"PRId64" at %s is still interlaced\n",
               outlink->frame_count, av_ts2timestr(in->pts, &inlink->time_base));
        dst->top_field_first = field;
    }
    av_log(ctx, AV_LOG_DEBUG, "SC:%d | COMBS: %3d %3d %3d %3d %3d (combpel=%d)"
           " match=%d combed=%s\n", sc, combs[0], combs[1], combs[2], combs[3], combs[4],
           fm->combpel, match, dst->interlaced_frame ? "YES" : "NO");
    return ff_filter_frame(outlink, dst);
}

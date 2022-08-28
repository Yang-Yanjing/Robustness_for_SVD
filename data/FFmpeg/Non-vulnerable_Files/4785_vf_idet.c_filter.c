static void filter(AVFilterContext *ctx)
{
    IDETContext *idet = ctx->priv;
    int y, i;
    int64_t alpha[2]={0};
    int64_t delta=0;
    int64_t gamma[2]={0};
    Type type, best_type;
    RepeatedField repeat;
    int match = 0;
    AVDictionary **metadata = avpriv_frame_get_metadatap(idet->cur);
    for (i = 0; i < idet->csp->nb_components; i++) {
        int w = idet->cur->width;
        int h = idet->cur->height;
        int refs = idet->cur->linesize[i];
        if (i && i<3) {
            w = FF_CEIL_RSHIFT(w, idet->csp->log2_chroma_w);
            h = FF_CEIL_RSHIFT(h, idet->csp->log2_chroma_h);
        }
        for (y = 2; y < h - 2; y++) {
            uint8_t *prev = &idet->prev->data[i][y*refs];
            uint8_t *cur  = &idet->cur ->data[i][y*refs];
            uint8_t *next = &idet->next->data[i][y*refs];
            alpha[ y   &1] += idet->filter_line(cur-refs, prev, cur+refs, w);
            alpha[(y^1)&1] += idet->filter_line(cur-refs, next, cur+refs, w);
            delta          += idet->filter_line(cur-refs,  cur, cur+refs, w);
            gamma[(y^1)&1] += idet->filter_line(cur     , prev, cur     , w);
        }
    }
    if      (alpha[0] > idet->interlace_threshold * alpha[1]){
        type = TFF;
    }else if(alpha[1] > idet->interlace_threshold * alpha[0]){
        type = BFF;
    }else if(alpha[1] > idet->progressive_threshold * delta){
        type = PROGRESSIVE;
    }else{
        type = UNDETERMINED;
    }
    if ( gamma[0] > idet->repeat_threshold * gamma[1] ){
        repeat = REPEAT_TOP;
    } else if ( gamma[1] > idet->repeat_threshold * gamma[0] ){
        repeat = REPEAT_BOTTOM;
    } else {
        repeat = REPEAT_NONE;
    }
    memmove(idet->history+1, idet->history, HIST_SIZE-1);
    idet->history[0] = type;
    best_type = UNDETERMINED;
    for(i=0; i<HIST_SIZE; i++){
        if(idet->history[i] != UNDETERMINED){
            if(best_type == UNDETERMINED)
                best_type = idet->history[i];
            if(idet->history[i] == best_type) {
                match++;
            }else{
                match=0;
                break;
            }
        }
    }
    if(idet->last_type == UNDETERMINED){
        if(match  ) idet->last_type = best_type;
    }else{
        if(match>2) idet->last_type = best_type;
    }
    if      (idet->last_type == TFF){
        idet->cur->top_field_first = 1;
        idet->cur->interlaced_frame = 1;
    }else if(idet->last_type == BFF){
        idet->cur->top_field_first = 0;
        idet->cur->interlaced_frame = 1;
    }else if(idet->last_type == PROGRESSIVE){
        idet->cur->interlaced_frame = 0;
    }
    for(i=0; i<3; i++)
        idet->repeats[i]  = av_rescale(idet->repeats [i], idet->decay_coefficient, PRECISION);
    for(i=0; i<4; i++){
        idet->prestat [i] = av_rescale(idet->prestat [i], idet->decay_coefficient, PRECISION);
        idet->poststat[i] = av_rescale(idet->poststat[i], idet->decay_coefficient, PRECISION);
    }
    idet->total_repeats [         repeat] ++;
    idet->repeats       [         repeat] += PRECISION;
    idet->total_prestat [           type] ++;
    idet->prestat       [           type] += PRECISION;
    idet->total_poststat[idet->last_type] ++;
    idet->poststat      [idet->last_type] += PRECISION;
    av_log(ctx, AV_LOG_DEBUG, "Repeated Field:%12s, Single frame:%12s, Multi frame:%12s\n",
           rep2str(repeat), type2str(type), type2str(idet->last_type));
    av_dict_set    (metadata, "lavfi.idet.repeated.current_frame", rep2str(repeat), 0);
    av_dict_set_fxp(metadata, "lavfi.idet.repeated.neither",       idet->repeats[REPEAT_NONE], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.repeated.top",           idet->repeats[REPEAT_TOP], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.repeated.bottom",        idet->repeats[REPEAT_BOTTOM], 2, 0);
    av_dict_set    (metadata, "lavfi.idet.single.current_frame",   type2str(type), 0);
    av_dict_set_fxp(metadata, "lavfi.idet.single.tff",             idet->prestat[TFF], 2 , 0);
    av_dict_set_fxp(metadata, "lavfi.idet.single.bff",             idet->prestat[BFF], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.single.progressive",     idet->prestat[PROGRESSIVE], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.single.undetermined",    idet->prestat[UNDETERMINED], 2, 0);
    av_dict_set    (metadata, "lavfi.idet.multiple.current_frame", type2str(idet->last_type), 0);
    av_dict_set_fxp(metadata, "lavfi.idet.multiple.tff",           idet->poststat[TFF], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.multiple.bff",           idet->poststat[BFF], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.multiple.progressive",   idet->poststat[PROGRESSIVE], 2, 0);
    av_dict_set_fxp(metadata, "lavfi.idet.multiple.undetermined",  idet->poststat[UNDETERMINED], 2, 0);
}

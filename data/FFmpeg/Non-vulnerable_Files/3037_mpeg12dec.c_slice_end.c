static int slice_end(AVCodecContext *avctx, AVFrame *pict)
{
    Mpeg1Context *s1  = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    if (!s1->mpeg_enc_ctx_allocated || !s->current_picture_ptr)
        return 0;
    if (s->avctx->hwaccel) {
        int ret = s->avctx->hwaccel->end_frame(s->avctx);
        if (ret < 0) {
            av_log(avctx, AV_LOG_ERROR,
                   "hardware accelerator failed to decode picture\n");
            return ret;
        }
    }
    
    if ( !s->first_field && !s1->first_slice) {
        
        ff_er_frame_end(&s->er);
        ff_mpv_frame_end(s);
        if (s->pict_type == AV_PICTURE_TYPE_B || s->low_delay) {
            int ret = av_frame_ref(pict, s->current_picture_ptr->f);
            if (ret < 0)
                return ret;
            ff_print_debug_info(s, s->current_picture_ptr, pict);
            ff_mpv_export_qp_table(s, pict, s->current_picture_ptr, FF_QSCALE_TYPE_MPEG2);
        } else {
            if (avctx->active_thread_type & FF_THREAD_FRAME)
                s->picture_number++;
            
            
            if (s->last_picture_ptr) {
                int ret = av_frame_ref(pict, s->last_picture_ptr->f);
                if (ret < 0)
                    return ret;
                ff_print_debug_info(s, s->last_picture_ptr, pict);
                ff_mpv_export_qp_table(s, pict, s->last_picture_ptr, FF_QSCALE_TYPE_MPEG2);
            }
        }
        return 1;
    } else {
        return 0;
    }
}

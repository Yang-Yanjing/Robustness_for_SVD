static void write_slice_end(MpegEncContext *s){
    if(CONFIG_MPEG4_ENCODER && s->codec_id==AV_CODEC_ID_MPEG4){
        if(s->partitioned_frame){
            ff_mpeg4_merge_partitions(s);
        }
        ff_mpeg4_stuffing(&s->pb);
    }else if(CONFIG_MJPEG_ENCODER && s->out_format == FMT_MJPEG){
        ff_mjpeg_encode_stuffing(s);
    }
    avpriv_align_put_bits(&s->pb);
    flush_put_bits(&s->pb);
    if ((s->avctx->flags & AV_CODEC_FLAG_PASS1) && !s->partitioned_frame)
        s->misc_bits+= get_bits_diff(s);
}
}
static void write_slice_end(MpegEncContext *s){
    if(CONFIG_MPEG4_ENCODER && s->codec_id==AV_CODEC_ID_MPEG4){
        if(s->partitioned_frame){
            ff_mpeg4_merge_partitions(s);
        }
        ff_mpeg4_stuffing(&s->pb);
    }else if(CONFIG_MJPEG_ENCODER && s->out_format == FMT_MJPEG){
        ff_mjpeg_encode_stuffing(s);
    }
    avpriv_align_put_bits(&s->pb);
    flush_put_bits(&s->pb);
    if ((s->avctx->flags & AV_CODEC_FLAG_PASS1) && !s->partitioned_frame)
        s->misc_bits+= get_bits_diff(s);
}

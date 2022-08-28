av_cold int ff_mpv_encode_end(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;
    int i;
    ff_rate_control_uninit(s);
    ff_mpv_common_end(s);
    if (CONFIG_MJPEG_ENCODER &&
        s->out_format == FMT_MJPEG)
        ff_mjpeg_encode_close(s);
    av_freep(&avctx->extradata);
    for (i = 0; i < FF_ARRAY_ELEMS(s->tmp_frames); i++)
        av_frame_free(&s->tmp_frames[i]);
    ff_free_picture_tables(&s->new_picture);
    ff_mpeg_unref_picture(s->avctx, &s->new_picture);
    av_freep(&s->avctx->stats_out);
    av_freep(&s->ac_stats);
    if(s->q_chroma_intra_matrix   != s->q_intra_matrix  ) av_freep(&s->q_chroma_intra_matrix);
    if(s->q_chroma_intra_matrix16 != s->q_intra_matrix16) av_freep(&s->q_chroma_intra_matrix16);
    s->q_chroma_intra_matrix=   NULL;
    s->q_chroma_intra_matrix16= NULL;
    av_freep(&s->q_intra_matrix);
    av_freep(&s->q_inter_matrix);
    av_freep(&s->q_intra_matrix16);
    av_freep(&s->q_inter_matrix16);
    av_freep(&s->input_picture);
    av_freep(&s->reordered_input_picture);
    av_freep(&s->dct_offset);
    return 0;
}

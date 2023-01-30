static void update_duplicate_context_after_me(MpegEncContext *dst,
                                              MpegEncContext *src)
{
#define COPY(a) dst->a= src->a
    COPY(pict_type);
    COPY(current_picture);
    COPY(f_code);
    COPY(b_code);
    COPY(qscale);
    COPY(lambda);
    COPY(lambda2);
    COPY(picture_in_gop_number);
    COPY(gop_picture_number);
    COPY(frame_pred_frame_dct); 
    COPY(progressive_frame);    
    COPY(partitioned_frame);    
#undef COPY
}

static inline void encode_mb_hq(MpegEncContext *s, MpegEncContext *backup, MpegEncContext *best, int type,
                           PutBitContext pb[2], PutBitContext pb2[2], PutBitContext tex_pb[2],
                           int *dmin, int *next_block, int motion_x, int motion_y)
{
    int score;
    uint8_t *dest_backup[3];
    copy_context_before_encode(s, backup, type);
    s->block= s->blocks[*next_block];
    s->pb= pb[*next_block];
    if(s->data_partitioning){
        s->pb2   = pb2   [*next_block];
        s->tex_pb= tex_pb[*next_block];
    }
    if(*next_block){
        memcpy(dest_backup, s->dest, sizeof(s->dest));
        s->dest[0] = s->sc.rd_scratchpad;
        s->dest[1] = s->sc.rd_scratchpad + 16*s->linesize;
        s->dest[2] = s->sc.rd_scratchpad + 16*s->linesize + 8;
        av_assert0(s->linesize >= 32); 
    }
    encode_mb(s, motion_x, motion_y);
    score= put_bits_count(&s->pb);
    if(s->data_partitioning){
        score+= put_bits_count(&s->pb2);
        score+= put_bits_count(&s->tex_pb);
    }
    if(s->avctx->mb_decision == FF_MB_DECISION_RD){
        ff_mpv_decode_mb(s, s->block);
        score *= s->lambda2;
        score += sse_mb(s) << FF_LAMBDA_SHIFT;
    }
    if(*next_block){
        memcpy(s->dest, dest_backup, sizeof(s->dest));
    }
    if(score<*dmin){
        *dmin= score;
        *next_block^=1;
        copy_context_after_encode(best, s, type);
    }
}

static inline void copy_context_before_encode(MpegEncContext *d, MpegEncContext *s, int type){
    int i;
    memcpy(d->last_mv, s->last_mv, 2*2*2*sizeof(int)); 
    
    d->mb_skip_run= s->mb_skip_run;
    for(i=0; i<3; i++)
        d->last_dc[i] = s->last_dc[i];
    
    d->mv_bits= s->mv_bits;
    d->i_tex_bits= s->i_tex_bits;
    d->p_tex_bits= s->p_tex_bits;
    d->i_count= s->i_count;
    d->f_count= s->f_count;
    d->b_count= s->b_count;
    d->skip_count= s->skip_count;
    d->misc_bits= s->misc_bits;
    d->last_bits= 0;
    d->mb_skipped= 0;
    d->qscale= s->qscale;
    d->dquant= s->dquant;
    d->esc3_level_length= s->esc3_level_length;
}

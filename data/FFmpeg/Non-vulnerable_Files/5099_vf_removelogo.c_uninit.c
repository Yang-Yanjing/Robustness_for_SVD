static av_cold void uninit(AVFilterContext *ctx)
{
    RemovelogoContext *s = ctx->priv;
    int a, b;
    av_freep(&s->full_mask_data);
    av_freep(&s->half_mask_data);
    if (s->mask) {
        
        for (a = 0; a <= s->max_mask_size; a++) {
            
            for (b = -a; b <= a; b++) {
                av_freep(&s->mask[a][b + a]); 
            }
            av_freep(&s->mask[a]);
        }
        
        av_freep(&s->mask);
    }
}

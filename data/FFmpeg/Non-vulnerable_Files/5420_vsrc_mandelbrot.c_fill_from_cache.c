static void fill_from_cache(AVFilterContext *ctx, uint32_t *color, int *in_cidx, int *out_cidx, double py, double scale){
    MBContext *mb = ctx->priv;
    if(mb->morphamp)
        return;
    for(; *in_cidx < mb->cache_used; (*in_cidx)++){
        Point *p= &mb->point_cache[*in_cidx];
        int x;
        if(p->p[1] > py)
            break;
        x= round((p->p[0] - mb->start_x) / scale + mb->w/2);
        if(x<0 || x >= mb->w)
            continue;
        if(color) color[x] = p->val;
        if(out_cidx && *out_cidx < mb->cache_allocated)
            mb->next_cache[(*out_cidx)++]= *p;
    }
}

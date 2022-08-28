static int add_interval(struct ws_intervals *inter,
                        enum ws_interval_type type, uint32_t channels, int ref,
                        int64_t ts1, int32_t f1, int32_t a1,
                        int64_t ts2, int32_t f2, int32_t a2)
{
    struct ws_interval *i, *ri;
    if (ref >= 0) {
        ri = &inter->inter[ref];
        
        if (ri->type == type && ri->channels == channels &&
            ri->f1 == ri->f2 && ri->f2 == f1 && f1 == f2 &&
            ri->a1 == ri->a2 && ri->a2 == a1 && a1 == a2 &&
            ri->ts2 == ts1) {
            ri->ts2 = ts2;
            return ref;
        }
    }
    i = alloc_array_elem((void **)&inter->inter, sizeof(*i),
                         &inter->nb_inter, &inter->max_inter);
    if (!i)
        return AVERROR(ENOMEM);
    i->ts1      = ts1;
    i->ts2      = ts2;
    i->type     = type;
    i->channels = channels;
    i->f1       = f1;
    i->f2       = f2;
    i->a1       = a1;
    i->a2       = a2;
    i->phi      = ref >= 0 ? ref | 0x80000000 : 0;
    return i - inter->inter;
}

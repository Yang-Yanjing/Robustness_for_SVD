static PullupFrame *pullup_get_frame(PullupContext *s)
{
    PullupFrame *fr = &s->frame;
    int i, n = decide_frame_length(s);
    int aff = s->first->next->affinity;
    av_assert1(n < FF_ARRAY_ELEMS(fr->ifields));
    if (!n || fr->lock)
        return NULL;
    fr->lock++;
    fr->length = n;
    fr->parity = s->first->parity;
    fr->buffer = 0;
    for (i = 0; i < n; i++) {
        
        fr->ifields[i]   = s->first->buffer;
        s->first->buffer = 0;
        s->first         = s->first->next;
    }
    if (n == 1) {
        fr->ofields[fr->parity    ] = fr->ifields[0];
        fr->ofields[fr->parity ^ 1] = 0;
    } else if (n == 2) {
        fr->ofields[fr->parity    ] = fr->ifields[0];
        fr->ofields[fr->parity ^ 1] = fr->ifields[1];
    } else if (n == 3) {
        if (!aff)
            aff = (fr->ifields[0] == fr->ifields[1]) ? -1 : 1;
        fr->ofields[fr->parity    ] = fr->ifields[1 + aff];
        fr->ofields[fr->parity ^ 1] = fr->ifields[1      ];
    }
    pullup_lock_buffer(fr->ofields[0], 0);
    pullup_lock_buffer(fr->ofields[1], 1);
    if (fr->ofields[0] == fr->ofields[1]) {
        fr->buffer = fr->ofields[0];
        pullup_lock_buffer(fr->buffer, 2);
        return fr;
    }
    return fr;
}

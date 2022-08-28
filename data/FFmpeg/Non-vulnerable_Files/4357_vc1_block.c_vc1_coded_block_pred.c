static inline int vc1_coded_block_pred(MpegEncContext * s, int n,
                                       uint8_t **coded_block_ptr)
{
    int xy, wrap, pred, a, b, c;
    xy   = s->block_index[n];
    wrap = s->b8_stride;
    


    a = s->coded_block[xy - 1       ];
    b = s->coded_block[xy - 1 - wrap];
    c = s->coded_block[xy     - wrap];
    if (b == c) {
        pred = a;
    } else {
        pred = c;
    }
    
    *coded_block_ptr = &s->coded_block[xy];
    return pred;
}

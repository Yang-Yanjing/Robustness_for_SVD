static void write_element_header(AlacEncodeContext *s,
                                 enum AlacRawDataBlockType element,
                                 int instance)
{
    int encode_fs = 0;
    if (s->frame_size < DEFAULT_FRAME_SIZE)
        encode_fs = 1;
    put_bits(&s->pbctx, 3,  element);               
    put_bits(&s->pbctx, 4,  instance);              
    put_bits(&s->pbctx, 12, 0);                     
    put_bits(&s->pbctx, 1,  encode_fs);             
    put_bits(&s->pbctx, 2,  s->extra_bits >> 3);    
    put_bits(&s->pbctx, 1,  s->verbatim);           
    if (encode_fs)
        put_bits32(&s->pbctx, s->frame_size);       
}

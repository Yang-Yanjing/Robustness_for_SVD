static void mpeg_decode_quant_matrix_extension(MpegEncContext *s)
{
    ff_dlog(s->avctx, "matrix extension\n");
    if (get_bits1(&s->gb))
        load_matrix(s, s->chroma_intra_matrix, s->intra_matrix, 1);
    if (get_bits1(&s->gb))
        load_matrix(s, s->chroma_inter_matrix, s->inter_matrix, 0);
    if (get_bits1(&s->gb))
        load_matrix(s, s->chroma_intra_matrix, NULL, 1);
    if (get_bits1(&s->gb))
        load_matrix(s, s->chroma_inter_matrix, NULL, 0);
}

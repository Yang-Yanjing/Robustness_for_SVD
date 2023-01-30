}
static void EsOutESVarUpdate( es_out_t *out, es_out_id_t *es,
                              bool b_delete )
{
    EsOutESVarUpdateGeneric( out, es->i_id, &es->fmt, es->psz_language, b_delete );
}

}
static void EsOutDestroy( es_out_t *p_out )
{
    free( p_out->p_sys );
    free( p_out );
}

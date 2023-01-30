 *****************************************************************************/
static void SchroFrameFree( SchroFrame *frame, void *priv)
{
    struct picture_free_t *p_free = priv;
    if( !p_free )
        return;
    decoder_DeletePicture( p_free->p_dec, p_free->p_pic );
    free(p_free);
    (void)frame;
}

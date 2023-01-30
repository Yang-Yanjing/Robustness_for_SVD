 *****************************************************************************/
static void DecoderUpdateFormatLocked( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_assert_locked( &p_owner->lock );
    p_owner->b_fmt_description = true;
    /* Copy es_format */
    es_format_Clean( &p_owner->fmt_description );
    es_format_Copy( &p_owner->fmt_description, &p_dec->fmt_out );
    /* Move p_description */
    if( p_owner->p_description && p_dec->p_description )
        vlc_meta_Delete( p_owner->p_description );
    p_owner->p_description = p_dec->p_description;
    p_dec->p_description = NULL;
}

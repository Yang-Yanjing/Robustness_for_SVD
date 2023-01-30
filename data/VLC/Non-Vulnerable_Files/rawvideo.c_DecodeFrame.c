 *****************************************************************************/
static picture_t *DecodeFrame( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_block = DecodeBlock( p_dec, pp_block );
    if( p_block == NULL )
        return NULL;
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Get a new picture */
    picture_t *p_pic = decoder_NewPicture( p_dec );
    if( p_pic == NULL )
    {
        block_Release( p_block );
        return NULL;
    }
    FillPicture( p_dec, p_block, p_pic );
    /* Date management: 1 frame per packet */
    p_pic->date = date_Get( &p_dec->p_sys->pts );
    date_Increment( &p_sys->pts, 1 );
    if( p_block->i_flags & BLOCK_FLAG_INTERLACED_MASK )
    {
        p_pic->b_progressive = false;
        p_pic->i_nb_fields = 2;
        if( p_block->i_flags & BLOCK_FLAG_TOP_FIELD_FIRST )
            p_pic->b_top_field_first = true;
        else
            p_pic->b_top_field_first = false;
    }
    else
        p_pic->b_progressive = true;
    block_Release( p_block );
    return p_pic;
}

}
static subpicture_t *Convert( decoder_t *p_dec, block_t *p_block )
{
    assert( p_block );
    decoder_sys_t *p_sys = p_dec->p_sys;
    const int64_t i_pts = p_block->i_pts;
    bool b_changed = false;
    /* TODO do the real decoding here */
    while( p_block->i_buffer >= 3 )
    {
        if( p_block->p_buffer[0] == p_sys->i_field )
            b_changed |= Eia608Parse( &p_sys->eia608, p_sys->i_channel, &p_block->p_buffer[1] );
        p_block->i_buffer -= 3;
        p_block->p_buffer += 3;
    }
    if( p_block )
        block_Release( p_block );
    if( b_changed )
    {
        char *psz_subtitle = Eia608Text( &p_sys->eia608, false );
        char *psz_html = Eia608Text( &p_sys->eia608, true );
        return Subtitle( p_dec, psz_subtitle, psz_html, i_pts );
    }
    return NULL;
}

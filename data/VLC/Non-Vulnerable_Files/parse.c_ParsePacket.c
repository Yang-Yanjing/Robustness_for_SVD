 *****************************************************************************/
subpicture_t * ParsePacket( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t *p_spu;
    subpicture_data_t spu_data;
    spu_properties_t spu_properties;
    /* Allocate the subpicture internal data. */
    p_spu = decoder_NewSubpicture( p_dec, NULL );
    if( !p_spu ) return NULL;
    p_spu->i_original_picture_width =
        p_dec->fmt_in.subs.spu.i_original_frame_width;
    p_spu->i_original_picture_height =
        p_dec->fmt_in.subs.spu.i_original_frame_height;
    /* Getting the control part */
    if( ParseControlSeq( p_dec, p_spu, &spu_data, &spu_properties, p_sys->i_pts ) )
    {
        /* There was a parse error, delete the subpicture */
        decoder_DeleteSubpicture( p_dec, p_spu );
        return NULL;
    }
    /* we are going to expand the RLE stuff so that we won't need to read
     * nibbles later on. This will speed things up a lot. Plus, we'll only
     * need to do this stupid interlacing stuff once.
     *
     * Rationale for the "p_spudec->i_rle_size * 4*sizeof(*spu_data.p_data)":
     *  one byte gaves two nibbles and may be used twice (once per field)
     * generating 4 codes.
     */
    spu_data.p_data = malloc( sizeof(*spu_data.p_data) * 2 * 2 * p_sys->i_rle_size );
    /* We try to display it */
    if( ParseRLE( p_dec, &spu_data, &spu_properties ) )
    {
        /* There was a parse error, delete the subpicture */
        decoder_DeleteSubpicture( p_dec, p_spu );
        free( spu_data.p_data );
        return NULL;
    }
#ifdef DEBUG_SPUDEC
    msg_Dbg( p_dec, "total size: 0x%x, RLE offsets: 0x%x 0x%x",
             p_sys->i_spu_size,
             spu_data.pi_offset[0], spu_data.pi_offset[1] );
#endif
    Render( p_dec, p_spu, &spu_data, &spu_properties );
    free( spu_data.p_data );
    return p_spu;
}

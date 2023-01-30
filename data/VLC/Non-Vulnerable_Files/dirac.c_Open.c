 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec !=  VLC_CODEC_DIRAC )
        return VLC_EGENERIC;
    p_dec->pf_packetize = Packetize;
    /* Create the output format */
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    p_dec->p_sys = p_sys = calloc( 1, sizeof( decoder_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_eu_pts = p_sys->i_eu_dts = VLC_TS_INVALID;
    p_sys->i_sync_pts = p_sys->i_sync_dts = VLC_TS_INVALID;
    p_sys->i_dts_last_out = p_sys->i_pts_last_out = VLC_TS_INVALID;
    p_sys->i_state = NOT_SYNCED;
    block_BytestreamInit( &p_sys->bytestream );
    p_sys->pp_outqueue_last = &p_sys->p_outqueue;
    p_sys->pp_eu_last = &p_sys->p_eu;
    date_Init( &p_sys->dts, 1, 1 );
    dirac_ReorderInit( &p_sys->reorder_buf );
    if( p_dec->fmt_in.i_extra > 0 )
    {
        /* handle hacky systems like ogg that dump some headers
         * in p_extra. and packetizers that expect it to be filled
         * in before real startup */
        block_t *p_init = block_Alloc( p_dec->fmt_in.i_extra );
        if( !p_init )
        {
            /* memory might be avaliable soon.  it isn't the end of
             * the world that fmt_in.i_extra isn't handled */
            return VLC_SUCCESS;
        }
        memcpy( p_init->p_buffer, p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra );
        /* in theory p_extra should contain just a seqhdr&EOS.  if just a
         * seqhdr, ensure it is extracted by appending an EOS with
         * prev_offset = seqhdr length, ie i_extra.  If all were actually
         * ok, this won't do anything bad */
        if( ( p_init->p_next = dirac_EmitEOS( p_dec, p_dec->fmt_in.i_extra ) ) )
        {
            /* to ensure that one of these two EOS dataunits gets extracted,
             * send a second one */
            p_init->p_next->p_next = dirac_EmitEOS( p_dec, 13 );
        }
        block_t *p_block;
        while( ( p_block = Packetize( p_dec, &p_init ) ) )
            block_Release( p_block );
    }
    return VLC_SUCCESS;
}

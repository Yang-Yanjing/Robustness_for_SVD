 *****************************************************************************/
static block_t *DecodePacket( decoder_t *p_dec, ogg_packet *p_oggpacket,
                              int i_nb_samples, int i_end_trim )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( !p_oggpacket->bytes )
        return NULL;
    int spp;
    spp=opus_packet_get_nb_frames(p_oggpacket->packet,p_oggpacket->bytes);
    if(spp>0)spp*=opus_packet_get_samples_per_frame(p_oggpacket->packet,48000);
    if(spp<120||spp>120*48)return NULL;
    /* Since the information isn't always available at the demux level
     * use the packet's sample number */
    if(!i_nb_samples)
        i_nb_samples = spp;
    block_t *p_aout_buffer=decoder_NewAudioBuffer( p_dec, spp );
    if ( !p_aout_buffer )
    {
        msg_Err(p_dec, "Oops: No new buffer was returned!");
        return NULL;
    }
    spp=opus_multistream_decode_float(p_sys->p_st, p_oggpacket->packet,
         p_oggpacket->bytes, (float *)p_aout_buffer->p_buffer, spp, 0);
    if( spp < 0 || i_nb_samples <= 0 || i_end_trim >= i_nb_samples)
    {
        block_Release(p_aout_buffer);
        if( spp < 0 )
            msg_Err( p_dec, "Error: corrupted stream?" );
        return NULL;
    }
    p_aout_buffer->i_buffer = (i_nb_samples - i_end_trim) *
                              p_sys->header.channels * sizeof(float);
    if( spp > i_nb_samples )
    {
        memmove(p_aout_buffer->p_buffer,
            p_aout_buffer->p_buffer
            + (spp - i_nb_samples)*p_sys->header.channels*sizeof(float),
            p_aout_buffer->i_buffer);
    }
    i_nb_samples -= i_end_trim;
#ifndef OPUS_SET_GAIN
    if(p_sys->header.gain!=0)
    {
        float gain = pow(10., p_sys->header.gain/5120.);
        float *buf =(float *)p_aout_buffer->p_buffer;
        int i;
        for( i = 0; i < i_nb_samples*p_sys->header.channels; i++)
            buf[i] *= gain;
    }
#endif
    p_aout_buffer->i_nb_samples = i_nb_samples;
    p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
    p_aout_buffer->i_length = date_Increment( &p_sys->end_date, i_nb_samples )
        - p_aout_buffer->i_pts;
    return p_aout_buffer;
}

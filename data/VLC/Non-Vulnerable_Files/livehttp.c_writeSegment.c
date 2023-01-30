}
static ssize_t writeSegment( sout_access_out_t *p_access )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    block_t *output = p_sys->block_buffer;
    p_sys->block_buffer = NULL;
    p_sys->last_block_buffer = &p_sys->block_buffer;
    ssize_t i_write=0;
    bool crypted = false;
    while( output )
    {
        if( p_sys->key_uri && !crypted )
        {
            if( p_sys->stuffing_size )
            {
                output = block_Realloc( output, p_sys->stuffing_size, output->i_buffer );
                if( unlikely(!output ) )
                    return VLC_ENOMEM;
                memcpy( output->p_buffer, p_sys->stuffing_bytes, p_sys->stuffing_size );
                p_sys->stuffing_size = 0;
            }
            size_t original = output->i_buffer;
            size_t padded = (output->i_buffer + 15 ) & ~15;
            size_t pad = padded - original;
            if( pad )
            {
                p_sys->stuffing_size = 16-pad;
                output->i_buffer -= p_sys->stuffing_size;
                memcpy(p_sys->stuffing_bytes, &output->p_buffer[output->i_buffer], p_sys->stuffing_size);
            }
            gcry_error_t err = gcry_cipher_encrypt( p_sys->aes_ctx,
                                output->p_buffer, output->i_buffer, NULL, 0 );
            if( err )
            {
                msg_Err( p_access, "Encryption failure: %s ", gpg_strerror(err) );
                return -1;
            }
            crypted=true;
        }
        ssize_t val = write( p_sys->i_handle, output->p_buffer, output->i_buffer );
        if ( val == -1 )
        {
           if ( errno == EINTR )
              continue;
           return -1;
        }
        p_sys->f_seglen =
            (float)(output->i_length +
                    output->i_dts - p_sys->i_opendts + p_sys->i_dts_offset) / CLOCK_FREQ;
        if ( (size_t)val >= output->i_buffer )
        {
           block_t *p_next = output->p_next;
           block_Release (output);
           output = p_next;
           crypted=false;
        }
        else
        {
           output->p_buffer += val;
           output->i_buffer -= val;
        }
        i_write += val;
    }
    return i_write;
}

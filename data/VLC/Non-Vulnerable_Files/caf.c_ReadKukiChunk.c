}
static int ReadKukiChunk( demux_t *p_demux, uint64_t i_size )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const uint8_t *p_peek;
    /* stream_Peek can't handle sizes bigger than INT32_MAX, and also p_sys->fmt.i_extra is of type 'int'*/
    if( i_size > INT32_MAX )
    {
        msg_Err( p_demux, "Magic Cookie chunk too big" );
        return VLC_EGENERIC;
    }
    if( (unsigned int)stream_Peek( p_demux->s, &p_peek, (int)i_size ) < i_size )
    {
        msg_Err( p_demux, "Couldn't peek extra data" );
        return VLC_EGENERIC;
    }
    if( p_sys->fmt.i_codec  == VLC_CODEC_ALAC )
    {
        int error = ProcessALACCookie( p_demux, p_peek, i_size );
        if( error ) return error;
    }
    else if( p_sys->fmt.i_codec == VLC_CODEC_MP4A )
    {
        int error = ProcessAACCookie( p_demux, p_peek, i_size );
        if( error ) return error;
    }
    else
    {
        p_sys->fmt.i_extra = (int)i_size;
        p_sys->fmt.p_extra = malloc( i_size );
        if( !p_sys->fmt.p_extra )
        {
            return VLC_ENOMEM;
        }
        memcpy( p_sys->fmt.p_extra, p_peek, p_sys->fmt.i_extra );
    }
    return VLC_SUCCESS;
}
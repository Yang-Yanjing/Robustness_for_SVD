}
static int Read( stream_t *s, void *buffer, unsigned i_read )
{
    stream_sys_t *p_sys = s->p_sys;
    int length = 0;
    if( p_sys->b_error )
        return 0;
    length = sms_Read( s, (uint8_t*) buffer, i_read );
    if( length < 0 )
        return 0;
    /* This call to sms_Read will increment p_sys->playback.index
     * in case the last chunk we read into is entirely read */
    sms_Read( s, NULL, 0 );
    p_sys->playback.boffset += length;
    if( (unsigned)length < i_read )
        msg_Warn( s, "could not read %i bytes, only %i!", i_read, length );
    return length;
}

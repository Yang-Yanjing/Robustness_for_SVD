 */
static int ProcessAACCookie( demux_t *p_demux, const uint8_t *p, uint64_t i_size )
{
    const uint8_t kAAC_ES_DESCR_TAG = 3;
    const uint8_t kAAC_DEC_CONFIG_DESCR_TAG = 4;
    const uint8_t kAAC_DEC_SPEC_INFO_TAG = 5;
    demux_sys_t *p_sys = p_demux->p_sys;
    uint64_t i_offset = 0;
    uint64_t i_kuki_size = 0;
    uint64_t i_tag_len;
    uint8_t i_tag;
    if( !AACCookieGetTag( &i_tag, p, &i_offset, i_size )) goto aac_kuki_finish;
    if( i_tag == kAAC_ES_DESCR_TAG )
    {
        if( !AACCookieTagLen( &i_tag_len, p, &i_offset, i_size )) goto aac_kuki_finish;
        if( !AACCookieChkLen( 3, i_size, i_offset )) goto aac_kuki_finish;
        i_offset += 2; /* don't care (ES ID) */
        uint8_t i_flags = *( p + i_offset++ );
        if( i_flags&0x80 )
        {
            if( !AACCookieChkLen( 2, i_size, i_offset )) goto aac_kuki_finish;
            i_offset += 2; /* don't care (dependance) */
        }
        if( i_flags&0x40 )
        {
            if( !AACCookieChkLen( 1, i_size, i_offset )) goto aac_kuki_finish;
            uint8_t i_url_len = *( p + i_offset++ );
            i_offset += i_url_len; /* don't care (url) */
        }
        if( i_flags&0x20 )
        {
            if( !AACCookieChkLen( 2, i_size, i_offset )) goto aac_kuki_finish;
            i_offset += 2; /* don't care (OCR) */
        }
        if( !AACCookieGetTag( &i_tag, p, &i_offset, i_size )) goto aac_kuki_finish;
    }
    if( i_tag != kAAC_DEC_CONFIG_DESCR_TAG )
        goto aac_kuki_finish;
    if( !AACCookieTagLen( &i_tag_len, p, &i_offset, i_size )) goto aac_kuki_finish;
    if( !AACCookieChkLen( 1 + 1 + 3 + 4 + 4, i_size, i_offset )) goto aac_kuki_finish;
    i_offset += ( 1 + 1 + 3 + 4 + 4 ); /* don't care */
    if( !AACCookieGetTag( &i_tag, p, &i_offset, i_size )) goto aac_kuki_finish;
    if( i_tag != kAAC_DEC_SPEC_INFO_TAG ) /* this is the one we need */
        goto aac_kuki_finish;
    if( !AACCookieTagLen( &i_tag_len, p, &i_offset, i_size )) goto aac_kuki_finish;
    if( i_offset + i_tag_len > i_size )
        goto aac_kuki_finish;
    i_kuki_size = i_tag_len;
aac_kuki_finish:
    if( !i_kuki_size )
    {
        msg_Warn( p_demux, "Error parsing aac cookie. Passing it on to the decoder as is and hoping for the best." );
        i_kuki_size = i_size;
        i_offset = 0;
    }
    p_sys->fmt.i_extra = (int)i_kuki_size;
    p_sys->fmt.p_extra = malloc( i_kuki_size );
    if( !p_sys->fmt.p_extra )
    {
        return VLC_ENOMEM;
    }
    memcpy( p_sys->fmt.p_extra, p + i_offset, i_kuki_size );
    return VLC_SUCCESS;
}

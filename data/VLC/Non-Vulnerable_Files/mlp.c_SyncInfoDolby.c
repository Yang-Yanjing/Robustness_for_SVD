 */
static int SyncInfoDolby( const uint8_t *p_buf )
{
    int bsid;
    /* Check synword */
    if( p_buf[0] != 0x0b || p_buf[1] != 0x77 )
        return 0;
    /* Check bsid */
    bsid = p_buf[5] >> 3;
    if( bsid > 16 )
        return 0;
    if( bsid <= 10 )
        return GetAc3Size( p_buf );
    else
        return GetEac3Size( p_buf );
}

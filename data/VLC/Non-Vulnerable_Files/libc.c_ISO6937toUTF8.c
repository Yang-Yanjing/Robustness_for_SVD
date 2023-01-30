};
static size_t ISO6937toUTF8( const unsigned char **inbuf, size_t *inbytesleft,
                             unsigned char **outbuf, size_t *outbytesleft )
{
    if( !inbuf || !(*inbuf) )
        return (size_t)(0);    /* Reset state requested */
    const unsigned char *iptr = *inbuf;
    const unsigned char *iend = iptr + *inbytesleft;
    unsigned char *optr = *outbuf;
    unsigned char *oend = optr + *outbytesleft;
    uint16_t ch;
    int err = 0;
    while ( iptr < iend )
    {
        if( *iptr < 0x80 )
        {
            if( optr >= oend )
            {
                err = E2BIG;
                break;    /* No space in outbuf */
            }
            *optr++ = *iptr++;
            continue;
        }
        if ( optr + 2 >= oend )
        {
            err = E2BIG;
            break;        /* No space in outbuf for multibyte char */
        }
        ch = to_ucs4[*iptr - 0x80];
        if( ch == 0xffff )
        {
            /* Composed character */
            if ( iptr + 1 >= iend )
            {
                err = EINVAL;
                break;    /* No next character */
            }
            if ( iptr[1] < 0x40 || iptr[1] >= 0x80 ||
                 !(ch = to_ucs4_comb[iptr[0] - 0xc1][iptr[1] - 0x40]) )
            {
                err = EILSEQ;
                break;   /* Illegal combination */
            }
            iptr += 2;
        }
        else
        {
            if ( !ch )
            {
                err = EILSEQ;
                break;
            }
            iptr++;
        }
        if ( ch < 0x800 )
        {
            optr[1] = 0x80 | (ch & 0x3f);
            optr[0] = 0xc0 | (ch >> 6);
            optr +=2;
        }
        else
        {
            optr[2] = 0x80 | (ch & 0x3f);
            ch >>= 6;
            optr[1] = 0x80 | (ch & 0x3f);
            optr[0] = 0xe0 | (ch >> 6);
            optr += 3;
        }
    }
    *inbuf = iptr;
    *outbuf = optr;
    *inbytesleft = iend - iptr;
    *outbytesleft = oend - optr;
    if( err )
    {
        errno = err;
        return (size_t)(-1);
    }
    return (size_t)(0);
}

}
static bool handshaking ( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "Reading protocol version" );
    rfbProtocolVersionMsg pv;
    if ( !read_exact( p_filter, p_sys->i_socket, pv,
                      sz_rfbProtocolVersionMsg ) )
    {
        msg_Err( p_filter, "Could not read version message" );
        return false;
    }
    pv[sz_rfbProtocolVersionMsg] = '\0'; /* pv size is sz_rfbProtocolVersionMsg+1 */
    msg_Dbg( p_filter, "Server version is %s", pv );
    strncpy(pv, "RFB 003.003\n", sz_rfbProtocolVersionMsg);
    if( !write_exact(p_filter, p_sys->i_socket, pv,
                     sz_rfbProtocolVersionMsg) )
    {
        msg_Err( p_filter, "Could not write version message" );
        return false;
    }
    msg_Dbg( p_filter, "Reading authentication scheme" );
    uint32_t i_authScheme;
    if( !read_exact( p_filter, p_sys->i_socket, (char*)&i_authScheme, 4 ) )
    {
        msg_Err( p_filter, "Could not read authentication scheme" );
        return false;
    }
    i_authScheme = htonl(i_authScheme);
    msg_Dbg( p_filter, "Authentication scheme = %x", i_authScheme );
    if ( i_authScheme == rfbConnFailed )
    {
        msg_Err( p_filter, "Connection rejected by server" );
        return false;
    }
    if (i_authScheme == rfbVncAuth)
    {
        unsigned char challenge[CHALLENGESIZE];
        if ( !read_exact( p_filter, p_sys->i_socket,
                          (char*)challenge, CHALLENGESIZE ) )
        {
            msg_Err( p_filter, "Could not read password challenge" );
            return false;
        }
        vnc_encrypt_bytes( challenge, p_sys->psz_passwd );
        if( !write_exact(p_filter, p_sys->i_socket,
                         (char*)challenge, CHALLENGESIZE ) )
        {
            msg_Err( p_filter, "Could not write password" );
            return false;
        }
        uint32_t i_authResult;
        if( !read_exact( p_filter, p_sys->i_socket, (char*)&i_authResult, 4 ) )
        {
            msg_Err( p_filter, "Could not read authentication result" );
            return false;
        }
        i_authResult = htonl(i_authResult);
        if (i_authResult != rfbVncAuthOK)
        {
            msg_Err( p_filter, "VNC authentication failed" );
            return false;
        }
    }
    msg_Dbg( p_filter, "Writing client init message" );
    rfbClientInitMsg ci;
    ci.shared = 1;
    if( !write_exact( p_filter, p_sys->i_socket,
                      (char*)&ci, sz_rfbClientInitMsg ) )
    {
        msg_Err( p_filter, "Could not write client init message" );
        return false;
    }
    msg_Dbg( p_filter, "Reading server init message" );
    rfbServerInitMsg si;
    if( !read_exact( p_filter, p_sys->i_socket,
                     (char*)&si, sz_rfbServerInitMsg ) )
    {
        msg_Err( p_filter, "Could not read server init message" );
        return false;
    }
    si.framebufferWidth = htons(si.framebufferWidth);
    si.framebufferHeight = htons(si.framebufferHeight);
    si.format.redMax = htons(si.format.redMax);
    si.format.greenMax = htons(si.format.greenMax);
    si.format.blueMax = htons(si.format.blueMax);
    p_sys->i_vnc_width = si.framebufferWidth;
    p_sys->i_vnc_height = si.framebufferHeight;
    msg_Dbg( p_filter, "Servers preferred pixelformat: "
                        "%ux%u, R(%u),G(%u),B(%u), %u bit, depht=%u, %s",
                        si.framebufferWidth,
                        si.framebufferHeight,
                        si.format.redMax,
                        si.format.greenMax,
                        si.format.blueMax,
                        si.format.bitsPerPixel,
                        si.format.depth,
                        si.format.trueColour ? "TrueColor" : "Not-TrueColor");
    uint32_t i_nameLength = htonl(si.nameLength);
    if( i_nameLength > MAX_VNC_SERVER_NAME_LENGTH )
    {
        msg_Err( p_filter, "Server name too long" );
        return false;
    }
    char s_ServerName[MAX_VNC_SERVER_NAME_LENGTH+1];
    msg_Dbg( p_filter, "Reading server name with size = %u", i_nameLength );
    if( !read_exact( p_filter, p_sys->i_socket, s_ServerName, i_nameLength ) )
    {
        msg_Err( p_filter, "Could not read server name" );
        return false;
    }
    s_ServerName[i_nameLength] = '\0';
    if( strcmp( s_ServerName, "VDR-OSD") == 0 )
    {
        msg_Dbg( p_filter, "Server is a VDR" );
        p_sys->b_alpha_from_vnc = true;
    }
    else
    {
        msg_Dbg( p_filter, "Server is a normal VNC" );
        p_sys->b_alpha_from_vnc = false;
    }
    msg_Dbg( p_filter, "Server init message read properly" );
    msg_Dbg( p_filter, "Server name is %s", s_ServerName );
    msg_Dbg( p_filter, "Writing SetPixelFormat message" );
    rfbSetPixelFormatMsg sp;
    sp.type = rfbSetPixelFormat;
    sp.pad1 = sp.pad2 = 0;
    sp.format.bitsPerPixel = 8;
    sp.format.depth = 8 ;
    sp.format.bigEndian = 1;
    sp.format.trueColour = 0;
    sp.format.redMax = htons(31);
    sp.format.greenMax = htons(31);
    sp.format.blueMax = htons(31);
    sp.format.redShift = 10;
    sp.format.greenShift = 5;
    sp.format.blueShift = 0;
    sp.format.pad1 = sp.format.pad2 = 0;
    if( !write_exact( p_filter, p_sys->i_socket,
                      (char*)&sp, sz_rfbSetPixelFormatMsg) )
    {
        msg_Err( p_filter, "Could not write SetPixelFormat message" );
        return false;
    }
    msg_Dbg( p_filter, "Writing SetEncodings message" );
    rfbSetEncodingsMsg se;
    se.type = rfbSetEncodings;
    se.pad = 0;
    se.nEncodings = htons( p_sys->b_alpha_from_vnc ? 3 : 2 );
    if( !write_exact( p_filter, p_sys->i_socket,
                      (char*)&se, sz_rfbSetEncodingsMsg) )
    {
        msg_Err( p_filter, "Could not write SetEncodings message begin" );
        return false;
    }
    uint32_t i_encoding;
    msg_Dbg( p_filter, "Writing SetEncodings rfbEncodingCopyRect" );
    i_encoding = htonl(rfbEncodingCopyRect);
    if( !write_exact( p_filter, p_sys->i_socket, (char*)&i_encoding, 4) )
    {
        msg_Err( p_filter, "Could not write encoding type rfbEncodingCopyRect." );
        return false;
    }
    msg_Dbg( p_filter, "Writing SetEncodings rfbEncodingRRE" );
    i_encoding = htonl(rfbEncodingRRE);
    if( !write_exact(p_filter, p_sys->i_socket, (char*)&i_encoding, 4) )
    {
        msg_Err( p_filter, "Could not write encoding type rfbEncodingRRE." );
        return false;
    }
    if( p_sys->b_alpha_from_vnc )
    {
        msg_Dbg( p_filter, "Writing SetEncodings rfbEncSpecialUseAlpha" );
        i_encoding = 0x00F0FFFF; /* rfbEncSpecialUseAlpha is 0xFFFFF000
                                  * before we swap it */
        if( !write_exact(p_filter, p_sys->i_socket, (char*)&i_encoding, 4) )
        {
            msg_Err( p_filter, "Could not write encoding type rfbEncSpecialUseAlpha." );
            return false;
        }
    }
    return true;
}

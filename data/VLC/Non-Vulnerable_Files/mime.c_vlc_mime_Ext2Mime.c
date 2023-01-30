};
const char *vlc_mime_Ext2Mime( const char *psz_url )
{
    char *psz_ext;
    psz_ext = strrchr( psz_url, '.' );
    if( psz_ext )
    {
        int i;
        for( i = 0; ext_mime[i].psz_ext[0] ; i++ )
        {
            if( !strcasecmp( ext_mime[i].psz_ext, psz_ext ) )
            {
                return ext_mime[i].psz_mime;
            }
        }
    }
    return "application/octet-stream";
}

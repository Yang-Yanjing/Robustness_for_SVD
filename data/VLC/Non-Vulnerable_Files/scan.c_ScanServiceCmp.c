}
static int ScanServiceCmp( const void *a, const void *b )
{
    scan_service_t *sa = *(scan_service_t**)a;
    scan_service_t *sb = *(scan_service_t**)b;
    if( sa->i_channel == sb->i_channel )
    {
        if( sa->psz_name && sb->psz_name )
            return strcmp( sa->psz_name, sb->psz_name );
        return 0;
    }
    if( sa->i_channel == -1 )
        return 1;
    else if( sb->i_channel == -1 )
        return -1;
    if( sa->i_channel < sb->i_channel )
        return -1;
    else if( sa->i_channel > sb->i_channel )
        return 1;
    return 0;
}

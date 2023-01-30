#undef M
static int OutputNew( sout_stream_t *p_stream,
                      const char *psz_muxer, const char *psz_prefix, const char *psz_extension  )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_file = NULL, *psz_tmp = NULL;
    char *psz_output = NULL;
    int i_count;
    if( asprintf( &psz_tmp, "%s%s%s",
                  psz_prefix, psz_extension ? "." : "", psz_extension ? psz_extension : "" ) < 0 )
    {
        goto error;
    }
    psz_file = config_StringEscape( psz_tmp );
    if( !psz_file )
    {
        free( psz_tmp );
        goto error;
    }
    free( psz_tmp );
    if( asprintf( &psz_output, "std{access=file{no-append,no-format},"
                  "mux='%s',dst='%s'}", psz_muxer, psz_file ) < 0 )

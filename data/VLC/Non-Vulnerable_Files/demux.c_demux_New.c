 *****************************************************************************/
demux_t *demux_New( vlc_object_t *p_obj, input_thread_t *p_parent_input,
                    const char *psz_access, const char *psz_demux,
                    const char *psz_location,
                    stream_t *s, es_out_t *out, bool b_quick )
{
    demux_t *p_demux = vlc_custom_create( p_obj, sizeof( *p_demux ), "demux" );
    if( unlikely(p_demux == NULL) )
        return NULL;
    p_demux->p_input = p_parent_input;
    p_demux->psz_access = strdup( psz_access );
    if( psz_demux[0] == '\0' )
        /* Take into account "demux" to be able to do :demux=dump */
        p_demux->psz_demux = var_InheritString( p_obj, "demux" );
    else
        p_demux->psz_demux = strdup( psz_demux );
    p_demux->psz_location = strdup( psz_location );
    p_demux->psz_file = get_path( psz_location ); /* parse URL */
    if( unlikely(p_demux->psz_access == NULL
              || p_demux->psz_demux == NULL
              || p_demux->psz_location == NULL) )
        goto error;
    if( !b_quick )
        msg_Dbg( p_obj, "creating demux: access='%s' demux='%s' "
                 "location='%s' file='%s'",
                 p_demux->psz_access, p_demux->psz_demux,
                 p_demux->psz_location, p_demux->psz_file );
    p_demux->s          = s;
    p_demux->out        = out;
    p_demux->pf_demux   = NULL;
    p_demux->pf_control = NULL;
    p_demux->p_sys      = NULL;
    p_demux->info.i_update = 0;
    p_demux->info.i_title  = 0;
    p_demux->info.i_seekpoint = 0;
    /* NOTE: Add only file without any problems here and with strong detection:
     * - no .mp3, .a52, ...
     *  - wav can't be added 'cause of a52 and dts in them as raw audio
     */
    static const struct { char ext[5]; char demux[9]; } exttodemux[] =
    {
        { "aiff", "aiff" },
        { "asf",  "asf" }, { "wmv",  "asf" }, { "wma",  "asf" },
        { "avi",  "avi" },
        { "au",   "au" },
        { "flac", "flac" },
        { "dv",   "dv" },
        { "drc",  "dirac" },
        { "m3u",  "m3u" },
        { "mkv",  "mkv" }, { "mka",  "mkv" }, { "mks",  "mkv" },
        { "mp4",  "mp4" }, { "m4a",  "mp4" }, { "mov",  "mp4" }, { "moov", "mp4" },
        { "nsv",  "nsv" },
        { "ogg",  "ogg" }, { "ogm",  "ogg" }, /* legacy Ogg */
        { "oga",  "ogg" }, { "spx",  "ogg" }, { "ogv", "ogg" },
        { "ogx",  "ogg" }, /*RFC5334*/
        { "opus", "ogg" }, /*draft-terriberry-oggopus-01*/
        { "pva",  "pva" },
        { "rm",   "avformat" },
        { "m4v",  "m4v" },
        { "h264", "h264" },
        { "voc",  "voc" },
        { "mid",  "smf" }, { "rmi",  "smf" }, { "kar", "smf" },
        { "",  "" },
    };
    /* Here, we don't mind if it does not work, it must be quick */
    static const struct { char ext[4]; char demux[5]; } exttodemux_quick[] =
    {
        { "mp3", "mpga" },
        { "ogg", "ogg" },
        { "wma", "asf" },
        { "", "" }
    };
    if( s != NULL )
    {
        const char *psz_ext;
        const char *psz_module = p_demux->psz_demux;
        if( !strcmp(psz_module, "any") && p_demux->psz_file != NULL
         && (psz_ext = strrchr( p_demux->psz_file, '.' )) != NULL )
        {
            psz_ext++; // skip '.'
            if( !b_quick )
            {
                for( unsigned i = 0; exttodemux[i].ext[0]; i++ )
                {
                    if( !strcasecmp( psz_ext, exttodemux[i].ext ) )
                    {
                        psz_module = exttodemux[i].demux;
                        break;
                    }
                }
            }
            else
            {
                for( unsigned i = 0; exttodemux_quick[i].ext[0]; i++ )
                {
                    if( !strcasecmp( psz_ext, exttodemux_quick[i].ext ) )
                    {
                        psz_module = exttodemux_quick[i].demux;
                        break;
                    }
                }
            }
        }
        /* ID3/APE tags will mess-up demuxer probing so we skip it here.
         * ID3/APE parsers will called later on in the demuxer to access the
         * skipped info. */
        while (SkipID3Tag( p_demux ))
          ;
        SkipAPETag( p_demux );
        p_demux->p_module =
            module_need( p_demux, "demux", psz_module,
                         !strcmp( psz_module, p_demux->psz_demux ) );
    }
    else
    {
        p_demux->p_module =
            module_need( p_demux, "access_demux", p_demux->psz_access, true );
    }
    if( p_demux->p_module == NULL )
        goto error;
    return p_demux;
error:
    free( p_demux->psz_file );
    free( p_demux->psz_location );
    free( p_demux->psz_demux );
    free( p_demux->psz_access );
    vlc_object_release( p_demux );
    return NULL;
}

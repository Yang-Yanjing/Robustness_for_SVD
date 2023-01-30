}
static int ScanDvbSNextFast( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    msg_Dbg( p_scan->p_obj, "Scan index %"PRId64, p_scan->i_index );
    int *pi_count = &p_scan->parameter.sat_info.i_count;
    if( !p_scan->parameter.sat_info.psz_name )
    {
        msg_Err( p_scan->p_obj, "no satellite selected" );
        return VLC_EGENERIC;
    }
    /* if there are no transponders in mem, laod from config file */
    if( !*pi_count )
    {
        DIR *p_dir;
        char *psz_dir = NULL;
        char *data_dir = config_GetDataDir();
        if( asprintf( &psz_dir, "%s" DIR_SEP "dvb" DIR_SEP "dvb-s", data_dir ) == -1 )
            psz_dir = NULL;
        free( data_dir );
        if( !psz_dir )
        {
            free( p_scan->parameter.sat_info.psz_name );
            return VLC_EGENERIC;
        }
        /* open config directory */
        if( !( p_dir = vlc_opendir( psz_dir ) ) )
        {
            msg_Err( p_scan->p_obj, "could not open satellite info directory (%s)", psz_dir );
            free( p_scan->parameter.sat_info.psz_name );
            return VLC_EGENERIC;
        }
        /* find the requested file in the directory */
        for( ; ; ) {
            const char *psz_filename = vlc_readdir( p_dir );
            if( psz_filename == NULL )
                break;
            if( !strncmp( p_scan->parameter.sat_info.psz_name, psz_filename, 20 ) )
            {
                if( asprintf( &p_scan->parameter.sat_info.psz_path, "%s" DIR_SEP "%s", psz_dir, psz_filename ) == -1 )
                    p_scan->parameter.sat_info.psz_path = NULL;
                break;
            }
        }
        closedir( p_dir );
        if( !p_scan->parameter.sat_info.psz_path )
        {
            msg_Err( p_scan->p_obj, "could not find satellite config (%s)", p_scan->parameter.sat_info.psz_name );
            free( p_scan->parameter.sat_info.psz_name );
            return VLC_EGENERIC;
        }
        msg_Dbg( p_scan->p_obj, "using satellite config file (%s)", p_scan->parameter.sat_info.psz_path );
        FILE *f = vlc_fopen( p_scan->parameter.sat_info.psz_path, "r" );
        /* parse file */
        if( f )
        {
            scan_dvbs_transponder_t *p_transponders = malloc( sizeof( scan_dvbs_transponder_t ) );
            char type;
            char psz_fec[3];
            int res;
            do
            {
                if ( ( res = fscanf( f, "%c %d %c %d %2s\n",
                            &type,
                            &p_transponders[*pi_count].i_frequency,
                            &p_transponders[*pi_count].c_polarization,
                            &p_transponders[*pi_count].i_symbol_rate,
                            psz_fec ) ) != 5 )
                {
                    msg_Dbg( p_scan->p_obj, "error parsing transponder from file" );
                    continue;
                }
                /* decode fec */
                char psz_fec_list[] = "1/22/33/44/55/66/77/88/9";
                char *p_fec = strstr( psz_fec_list, psz_fec );
                if ( !p_fec )
                    p_transponders[*pi_count].i_fec = 9;    /* FEC_AUTO */
                else
                    p_transponders[*pi_count].i_fec = 1 + ( ( p_fec-psz_fec_list ) / 3 );
                (*pi_count)++;
                p_transponders = realloc(p_transponders, ( *pi_count + 1 ) * sizeof( scan_dvbs_transponder_t ) );
            } while (res != EOF);
            msg_Dbg( p_scan->p_obj, "parsed %d transponders from config", *pi_count);
            fclose( f );
            p_scan->parameter.sat_info.p_transponders = p_transponders;
        }
        else
        {
            msg_Err( p_scan->p_obj, "failed to open satellite file (%s)", p_scan->parameter.sat_info.psz_path );
            free( p_scan->parameter.sat_info.psz_name );
            free( p_scan->parameter.sat_info.psz_path );
            return VLC_EGENERIC;
        }
        free( p_scan->parameter.sat_info.psz_name );
        free( p_scan->parameter.sat_info.psz_path );
    }
    if( p_scan->i_index < *pi_count )
    {
        /* setup params for scan */
        p_cfg->i_symbol_rate = p_scan->parameter.sat_info.p_transponders[p_scan->i_index].i_symbol_rate / 1000;
        p_cfg->i_frequency = p_scan->parameter.sat_info.p_transponders[p_scan->i_index].i_frequency;
        p_cfg->i_fec = p_scan->parameter.sat_info.p_transponders[p_scan->i_index].i_fec;
        p_cfg->c_polarization = p_scan->parameter.sat_info.p_transponders[p_scan->i_index].c_polarization;
        msg_Dbg( p_scan->p_obj,
                 "transponder [%"PRId64"/%d]: frequency=%d, symbolrate=%d, fec=%d, polarization=%c",
                 p_scan->i_index + 1,
                 *pi_count,
                 p_cfg->i_frequency,
                 p_cfg->i_symbol_rate,
                 p_cfg->i_fec,
                 p_cfg->c_polarization );
        *pf_pos = (double)p_scan->i_index / *pi_count;
        return VLC_SUCCESS;
    }
    if( p_scan->parameter.sat_info.p_transponders )
    {
        free( p_scan->parameter.sat_info.p_transponders );
        p_scan->parameter.sat_info.p_transponders = NULL;
    }
    return VLC_EGENERIC;
}

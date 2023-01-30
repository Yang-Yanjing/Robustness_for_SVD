}
static int Control (demux_t *demux, int query, va_list args)
{
    demux_sys_t *sys = demux->p_sys;
    switch (query)
    {
        case DEMUX_GET_POSITION:
        {
            double *pos = va_arg (args, double *);
            if (unlikely(sys->track_id >= sys->titlec)
             || (sys->titlev[sys->track_id]->i_length == 0))
                *pos = 0.;
            else
            {
                int offset = gme_tell (sys->emu);
                *pos = (double)offset
                    / (double)(sys->titlev[sys->track_id]->i_length / 1000);
            }
            return VLC_SUCCESS;
        }
        case DEMUX_SET_POSITION:
        {
            double pos = va_arg (args, double);
            if (unlikely(sys->track_id >= sys->titlec)
             || (sys->titlev[sys->track_id]->i_length == 0))
                break;
            int seek = (sys->titlev[sys->track_id]->i_length / 1000) * pos;
            if (gme_seek (sys->emu, seek))
                break;
            return VLC_SUCCESS;
        }
        case DEMUX_GET_LENGTH:
        {
            int64_t *v = va_arg (args, int64_t *);
            if (unlikely(sys->track_id >= sys->titlec)
             || (sys->titlev[sys->track_id]->i_length == 0))
                break;
            *v = sys->titlev[sys->track_id]->i_length;
            return VLC_SUCCESS;
        }
        case DEMUX_GET_TIME:
        {
            int64_t *v = va_arg (args, int64_t *);
            *v = gme_tell (sys->emu) * INT64_C(1000);
            return VLC_SUCCESS;
        }
        case DEMUX_SET_TIME:
        {
            int64_t v = va_arg (args, int64_t) / 1000;
            if (v > INT_MAX || gme_seek (sys->emu, v))
                break;
            return VLC_SUCCESS;
        }
        case DEMUX_GET_TITLE_INFO:
        {
            input_title_t ***titlev = va_arg (args, input_title_t ***);
            int *titlec = va_arg (args, int *);
            *(va_arg (args, int *)) = 0; /* Title offset */
            *(va_arg (args, int *)) = 0; /* Chapter offset */
            unsigned n = sys->titlec;
            *titlev = malloc (sizeof (**titlev) * n);
            if (unlikely(*titlev == NULL))
                n = 0;
            *titlec = n;
            for (unsigned i = 0; i < n; i++)
                (*titlev)[i] = vlc_input_title_Duplicate (sys->titlev[i]);
            return VLC_SUCCESS;
        }
        case DEMUX_SET_TITLE:
        {
            int track_id = va_arg (args, int);
            if (track_id >= gme_track_count (sys->emu))
                break;
            gme_start_track (sys->emu, track_id);
            demux->info.i_update |= INPUT_UPDATE_TITLE;
            demux->info.i_title = track_id;
            sys->track_id = track_id;
            return VLC_SUCCESS;
        }
    }
    return VLC_EGENERIC;
}

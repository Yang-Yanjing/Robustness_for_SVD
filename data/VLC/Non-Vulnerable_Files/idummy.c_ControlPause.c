}
static int ControlPause( demux_t *demux, int query, va_list args )
{
    demux_sys_t *p_sys = demux->p_sys;
    switch( query )
    {
        case DEMUX_GET_POSITION:
        {
            double *ppos = va_arg( args, double * );
            double pos;
            mtime_t now = mdate();
            pos = 1. + ((double)(now - p_sys->end) / (double)p_sys->length);
            *ppos = (pos <= 1.) ? pos : 1.;
            break;
        }
        case DEMUX_SET_POSITION:
        {
            double pos = va_arg( args, double );
            mtime_t now = mdate();
            p_sys->end = now + (p_sys->length * (1. - pos));
            break;
        }
        case DEMUX_GET_LENGTH:
        {
            mtime_t *plen = va_arg( args, mtime_t * );
            *plen = p_sys->length;
            break;
        }
        case DEMUX_GET_TIME:
        {
            mtime_t *ppos = va_arg( args, mtime_t * );
            *ppos = mdate() + p_sys->length - p_sys->end;
            break;
        }
        case DEMUX_SET_TIME:
        {
            mtime_t pos = va_arg( args, mtime_t );
            p_sys->end = mdate() + p_sys->length - pos;
            break;
        }
        case DEMUX_CAN_SEEK:
            *va_arg( args, bool * ) = true;
            break;
        default:
            return DemuxControl( demux, query, args );
    }
    return VLC_SUCCESS;
}

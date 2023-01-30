}
bool transcode_osd_add( sout_stream_t *p_stream, es_format_t *p_fmt,
                                sout_stream_id_sys_t *id)
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    msg_Dbg( p_stream, "creating osd transcoding from fcc=`%4.4s' "
             "to fcc=`%4.4s'", (char*)&p_fmt->i_codec,
             (char*)&p_sys->i_scodec );
    id->b_transcode = true;
    /* Create a fake OSD menu elementary stream */
    if( transcode_osd_new( p_stream, id ) )
    {
        msg_Err( p_stream, "cannot create osd chain" );
        return false;
    }
    p_sys->b_osd = true;
    return true;
}

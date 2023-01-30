 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Delete the callbacks */
    var_DelCallback( p_stream, CFG_PREFIX "height", HeightCallback, p_stream );
    var_DelCallback( p_stream, CFG_PREFIX "width", WidthCallback, p_stream );
    var_DelCallback( p_stream, CFG_PREFIX "alpha", alphaCallback, p_stream );
    var_DelCallback( p_stream, CFG_PREFIX "x", xCallback, p_stream );
    var_DelCallback( p_stream, CFG_PREFIX "y", yCallback, p_stream );
    free( p_sys->psz_id );
    free( p_sys );
}

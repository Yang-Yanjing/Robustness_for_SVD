 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    if( !p_pic ) return NULL;
    filter_sys_t *p_sys = p_filter->p_sys;
    int level = atomic_load( &p_sys->i_level );
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        msg_Warn( p_filter, "can't get output picture" );
        picture_Release( p_pic );
        return NULL;
    }
    switch( p_pic->format.i_chroma )
    {
        case VLC_CODEC_RGB24:
            RVPosterize( p_pic, p_outpic, false, level );
            break;
        case VLC_CODEC_RGB32:
            RVPosterize( p_pic, p_outpic, true, level );
            break;
        CASE_PLANAR_YUV_SQUARE
            PlanarYUVPosterize( p_pic, p_outpic, level );
            break;
        CASE_PACKED_YUV_422
            PackedYUVPosterize( p_pic, p_outpic, level );
            break;
        default:
            assert( false );
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}

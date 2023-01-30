#endif
int ioctl_GetCdText( vlc_object_t *p_object, const vcddev_t *p_vcddev,
                     vlc_meta_t ***ppp_tracks, int *pi_tracks )
{
    uint8_t *p_text;
    int i_text;
    if( p_vcddev->i_vcdimage_handle != -1 )
        return -1;
    if( CdTextRead( p_object, p_vcddev, &p_text, &i_text ) )
        return -1;
    CdTextParse( ppp_tracks, pi_tracks, p_text, i_text );
    free( p_text );
    return 0;
}

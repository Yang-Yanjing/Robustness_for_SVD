}
bool vlc_fourcc_IsYUV(vlc_fourcc_t fcc)
{
    for( unsigned i = 0; p_list_YUV[i]; i++ )
    {
        if( p_list_YUV[i] == fcc )
            return true;
    }
    return false;
}

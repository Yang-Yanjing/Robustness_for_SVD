 */
static HRESULT CreateDSBufferPCM( vlc_object_t *obj, aout_stream_sys_t *sys,
                                  vlc_fourcc_t *i_format, int i_channels,
                                  int i_rate, bool b_probe )
{
    HRESULT hr;
    unsigned i_nb_channels = popcount( i_channels );
    if( var_GetBool( obj, "directx-audio-float32" ) )
    {
        hr = CreateDSBuffer( obj, sys, VLC_CODEC_FL32, i_channels,
                             i_nb_channels, i_rate, b_probe );
        if( hr == DS_OK )
        {
            *i_format = VLC_CODEC_FL32;
            return DS_OK;
        }
    }
    hr = CreateDSBuffer( obj, sys, VLC_CODEC_S16N, i_channels, i_nb_channels,
                         i_rate, b_probe );
    if( hr == DS_OK )
    {
        *i_format = VLC_CODEC_S16N;
        return DS_OK;
    }
    return hr;
}
 */
static HRESULT CreateDSBufferPCM( vlc_object_t *obj, aout_stream_sys_t *sys,
                                  vlc_fourcc_t *i_format, int i_channels,
                                  int i_rate, bool b_probe )
{
    HRESULT hr;
    unsigned i_nb_channels = popcount( i_channels );
    if( var_GetBool( obj, "directx-audio-float32" ) )
    {
        hr = CreateDSBuffer( obj, sys, VLC_CODEC_FL32, i_channels,
                             i_nb_channels, i_rate, b_probe );
        if( hr == DS_OK )
        {
            *i_format = VLC_CODEC_FL32;
            return DS_OK;
        }
    }

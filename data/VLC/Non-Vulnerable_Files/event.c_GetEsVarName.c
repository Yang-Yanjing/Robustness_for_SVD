}
static const char *GetEsVarName( int i_cat )
{
    switch( i_cat )
    {
    case VIDEO_ES:
        return "video-es";
    case AUDIO_ES:
        return "audio-es";
    default:
        assert( i_cat == SPU_ES );
        return "spu-es";
    }
}

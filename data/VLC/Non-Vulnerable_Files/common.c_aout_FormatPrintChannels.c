 *****************************************************************************/
const char * aout_FormatPrintChannels( const audio_sample_format_t * p_format )
{
    switch ( p_format->i_physical_channels )
    {
    case AOUT_CHAN_LEFT:
    case AOUT_CHAN_RIGHT:
    case AOUT_CHAN_CENTER:
        if ( (p_format->i_original_channels & AOUT_CHAN_CENTER)
              || (p_format->i_original_channels
                   & (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT)) )
            return "Mono";
        else if ( p_format->i_original_channels & AOUT_CHAN_LEFT )
            return "Left";
        return "Right";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT:
        if ( p_format->i_original_channels & AOUT_CHAN_REVERSESTEREO )
        {
            if ( p_format->i_original_channels & AOUT_CHAN_DOLBYSTEREO )
                return "Dolby/Reverse";
            return "Stereo/Reverse";
        }
        else
        {
            if ( p_format->i_original_channels & AOUT_CHAN_DOLBYSTEREO )
                return "Dolby";
            else if ( p_format->i_original_channels & AOUT_CHAN_DUALMONO )
                return "Dual-mono";
            else if ( p_format->i_original_channels == AOUT_CHAN_CENTER )
                return "Stereo/Mono";
            else if ( !(p_format->i_original_channels & AOUT_CHAN_RIGHT) )
                return "Stereo/Left";
            else if ( !(p_format->i_original_channels & AOUT_CHAN_LEFT) )
                return "Stereo/Right";
            return "Stereo";
        }
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER:
        return "3F";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_REARCENTER:
        return "2F1R";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARCENTER:
        return "3F1R";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT:
        return "2F2R";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
          | AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT:
        return "2F2M";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT:
        return "3F2R";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT:
        return "3F2M";
    case AOUT_CHAN_CENTER | AOUT_CHAN_LFE:
        if ( (p_format->i_original_channels & AOUT_CHAN_CENTER)
              || (p_format->i_original_channels
                   & (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT)) )
            return "Mono/LFE";
        else if ( p_format->i_original_channels & AOUT_CHAN_LEFT )
            return "Left/LFE";
        return "Right/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_LFE:
        if ( p_format->i_original_channels & AOUT_CHAN_DOLBYSTEREO )
            return "Dolby/LFE";
        else if ( p_format->i_original_channels & AOUT_CHAN_DUALMONO )
            return "Dual-mono/LFE";
        else if ( p_format->i_original_channels == AOUT_CHAN_CENTER )
            return "Mono/LFE";
        else if ( !(p_format->i_original_channels & AOUT_CHAN_RIGHT) )
            return "Stereo/Left/LFE";
        else if ( !(p_format->i_original_channels & AOUT_CHAN_LEFT) )
            return "Stereo/Right/LFE";
         return "Stereo/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER | AOUT_CHAN_LFE:
        return "3F/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_REARCENTER
          | AOUT_CHAN_LFE:
        return "2F1R/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARCENTER | AOUT_CHAN_LFE:
        return "3F1R/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_LFE:
        return "2F2R/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
          | AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_LFE:
        return "2F2M/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_LFE:
        return "3F2R/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_LFE:
        return "3F2M/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_MIDDLELEFT
          | AOUT_CHAN_MIDDLERIGHT:
        return "3F2M2R";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_MIDDLELEFT
          | AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_LFE:
        return "3F2M2R/LFE";
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARCENTER | AOUT_CHAN_MIDDLELEFT
          | AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_LFE:
        return "3F2M1R/LFE";
    }
    return "ERROR";
}

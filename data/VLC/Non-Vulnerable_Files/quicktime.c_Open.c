 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
#ifdef __APPLE__
    OSErr err;
    SInt32 qtVersion, macosversion;
    err = Gestalt(gestaltQuickTimeVersion, &qtVersion);
    err = Gestalt(gestaltSystemVersion, &macosversion);
#ifndef NDEBUG
    msg_Dbg( p_this, "Mac OS version is %#lx", macosversion );
    msg_Dbg( p_this, "Quicktime version is %#lx", qtVersion );
#endif
    /* bail out. This plugin is soo Carbon, that it can't be used on 10.5 at all */
    msg_Info( p_dec, "Your Mac OS version is to new to use this plugin for anything." );
    return VLC_EGENERIC;
#endif
    switch( p_dec->fmt_in.i_codec )
    {
        case VLC_CODEC_H264:
        case VLC_CODEC_CINEPAK:
        case VLC_FOURCC('I','V','4','1'): /* Indeo Video IV */
        case VLC_FOURCC('i','v','4','1'): /* dto. */
#ifdef __APPLE__
        case VLC_FOURCC('p','x','l','t'): /* Pixlet */
#endif
        case VLC_CODEC_DV:
        case VLC_CODEC_SVQ3: /* Sorenson v3 */
    /*    case VLC_CODEC_SVQ1:  Sorenson v1
        case VLC_FOURCC('Z','y','G','o'):
        case VLC_FOURCC('V','P','3','1'):
        case VLC_FOURCC('3','I','V','1'): */
        case VLC_CODEC_QTRLE:
        case VLC_CODEC_RPZA:
#ifdef LOADER
        p_dec->p_sys = NULL;
        p_dec->pf_decode_video = DecodeVideo;
        p_dec->fmt_out.i_cat = VIDEO_ES;
        return VLC_SUCCESS;
#else
        return OpenVideo( p_dec );
#endif
#ifdef __APPLE__
        case VLC_FOURCC('I','L','B','C'): /* iLBC */
            if ((err != noErr) || (qtVersion < 0x07500000)) 
                return VLC_EGENERIC;
        case VLC_FOURCC('i','l','b','c'): /* iLBC */
            if ((err != noErr) || (qtVersion < 0x07500000)) 
                return VLC_EGENERIC;
#endif
        case VLC_CODEC_AMR_NB: /* 3GPP AMR audio */
        case VLC_FOURCC('s','a','m','b'): /* 3GPP AMR-WB audio */
        case VLC_CODEC_MP4A: /* MPEG-4 audio */
        case VLC_FOURCC('Q','D','M','C'): /* QDesign */
        case VLC_CODEC_QDM2: /* QDesign* 2 */
        case VLC_CODEC_QCELP: /* Qualcomm Purevoice Codec */
        case VLC_FOURCC('Q','C','L','P'): /* Qualcomm Purevoice Codec */
        case VLC_CODEC_MACE3: /* MACE3 audio decoder */
        case VLC_CODEC_MACE6: /* MACE6 audio decoder */
        case VLC_FOURCC('d','v','c','a'): /* DV Audio */
        case VLC_FOURCC('s','o','w','t'): /* 16-bit Little Endian */
        case VLC_FOURCC('t','w','o','s'): /* 16-bit Big Endian */
        case VLC_CODEC_ALAW: /* ALaw 2:1 */
        case VLC_FOURCC('u','l','a','w'): /* mu-Law 2:1 */
        case VLC_FOURCC('r','a','w',' '): /* 8-bit offset binaries */
        case VLC_CODEC_FL32: /* 32-bit Floating Point */
        case VLC_CODEC_FL64: /* 64-bit Floating Point */
        case VLC_FOURCC('i','n','2','4'): /* 24-bit Interger */
        case VLC_FOURCC('i','n','3','2'): /* 32-bit Integer */
        case 0x0011:                            /* DVI IMA */
        case 0x6D730002:                        /* Microsoft ADPCM-ACM */
        case 0x6D730011:                        /* DVI Intel IMAADPCM-ACM */
#ifdef LOADER
        p_dec->p_sys = NULL;
        p_dec->pf_decode_audio = DecodeAudio;
        p_dec->fmt_out.i_cat = AUDIO_ES;
        return VLC_SUCCESS;
#else
        return OpenAudio( p_dec );
#endif
        default:
            return VLC_EGENERIC;
    }
}

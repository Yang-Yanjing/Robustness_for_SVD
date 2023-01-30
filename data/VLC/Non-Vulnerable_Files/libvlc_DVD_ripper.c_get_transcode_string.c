}
const char* get_transcode_string(char *preset) {
    static const char mp4_high[] = "#transcode{vcodec=h264,venc=x264{cfr=16},scale=1,acodec=mp4a,ab=160,channels=2,samplerate=44100}";
    static const char mp4_low[] = "#transcode{vcodec=h264,venc=x264{cfr=40},scale=1,acodec=mp4a,ab=96,channels=2,samplerate=44100}";
    static const char ogg_high[] = "#transcode{vcodec=theo,venc=theora{quality=9},scale=1,acodec=vorb,ab=160,channels=2,samplerate=44100}";
    static const char ogg_low[] = "#transcode{vcodec=theo,venc=theora{quality=4},scale=1,acodec=vorb,ab=96,channels=2,samplerate=44100}";
    static const char webm_high[] = "#transcode{vcodec=VP80,vb=2000,scale=1,acodec=vorb,ab=160,channels=2,samplerate=44100}";
    static const char webm_low[] = "#transcode{vcodec=VP80,vb=1000,scale=1,acodec=vorb,ab=96,channels=2,samplerate=44100}";
    static const char nothing[] = "";
    if(0 == strcmp(preset, PRESET_H264_AAC_MP4_HIGH)) {
        return mp4_high;
    }
    else if(0 == strcmp(preset, PRESET_H264_AAC_MP4_LOW)) {
        return mp4_low;
    }
    else if(0 == strcmp(preset, PRESET_THEORA_VORBIS_OGG_HIGH)) {
        return ogg_high;
    }
    else if(0 == strcmp(preset, PRESET_THEORA_VORBIS_OGG_LOW)) {
        return ogg_low;
    }
    else if(0 == strcmp(preset, PRESET_VP8_VORBIS_WEBM_HIGH)) {
        return webm_high;
    }
    else if(0 == strcmp(preset, PRESET_VP8_VORBIS_WEBM_LOW)) {
        return webm_low;
    }
    else {
        return nothing;
    }
}
}
const char* get_transcode_string(char *preset) {
    static const char mp4_high[] = "#transcode{vcodec=h264,venc=x264{cfr=16},scale=1,acodec=mp4a,ab=160,channels=2,samplerate=44100}";
    static const char mp4_low[] = "#transcode{vcodec=h264,venc=x264{cfr=40},scale=1,acodec=mp4a,ab=96,channels=2,samplerate=44100}";
    static const char ogg_high[] = "#transcode{vcodec=theo,venc=theora{quality=9},scale=1,acodec=vorb,ab=160,channels=2,samplerate=44100}";
    static const char ogg_low[] = "#transcode{vcodec=theo,venc=theora{quality=4},scale=1,acodec=vorb,ab=96,channels=2,samplerate=44100}";
    static const char webm_high[] = "#transcode{vcodec=VP80,vb=2000,scale=1,acodec=vorb,ab=160,channels=2,samplerate=44100}";
    static const char webm_low[] = "#transcode{vcodec=VP80,vb=1000,scale=1,acodec=vorb,ab=96,channels=2,samplerate=44100}";
    static const char nothing[] = "";
    if(0 == strcmp(preset, PRESET_H264_AAC_MP4_HIGH)) {
        return mp4_high;
    }
    else if(0 == strcmp(preset, PRESET_H264_AAC_MP4_LOW)) {
        return mp4_low;
    }
    else if(0 == strcmp(preset, PRESET_THEORA_VORBIS_OGG_HIGH)) {
        return ogg_high;
    }
    else if(0 == strcmp(preset, PRESET_THEORA_VORBIS_OGG_LOW)) {
        return ogg_low;
    }
    else if(0 == strcmp(preset, PRESET_VP8_VORBIS_WEBM_HIGH)) {
        return webm_high;
    }
    else if(0 == strcmp(preset, PRESET_VP8_VORBIS_WEBM_LOW)) {
        return webm_low;
    }
    else {
        return nothing;
    }
}

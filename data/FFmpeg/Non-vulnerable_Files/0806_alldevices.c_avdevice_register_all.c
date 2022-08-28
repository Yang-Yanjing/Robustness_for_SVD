void avdevice_register_all(void)
{
    static int initialized;
    if (initialized)
        return;
    initialized = 1;
    
    REGISTER_INOUTDEV(ALSA,             alsa);
    REGISTER_INDEV   (AVFOUNDATION,     avfoundation);
    REGISTER_INDEV   (BKTR,             bktr);
    REGISTER_OUTDEV  (CACA,             caca);
    REGISTER_INOUTDEV(DECKLINK,         decklink);
    REGISTER_INDEV   (DSHOW,            dshow);
    REGISTER_INDEV   (DV1394,           dv1394);
    REGISTER_INOUTDEV(FBDEV,            fbdev);
    REGISTER_INDEV   (GDIGRAB,          gdigrab);
    REGISTER_INDEV   (IEC61883,         iec61883);
    REGISTER_INDEV   (JACK,             jack);
    REGISTER_INDEV   (LAVFI,            lavfi);
    REGISTER_INDEV   (OPENAL,           openal);
    REGISTER_OUTDEV  (OPENGL,           opengl);
    REGISTER_INOUTDEV(OSS,              oss);
    REGISTER_INOUTDEV(PULSE,            pulse);
    REGISTER_INDEV   (QTKIT,            qtkit);
    REGISTER_OUTDEV  (SDL,              sdl);
    REGISTER_INOUTDEV(SNDIO,            sndio);
    REGISTER_INOUTDEV(V4L2,             v4l2);

    REGISTER_INDEV   (VFWCAP,           vfwcap);
    REGISTER_INDEV   (X11GRAB,          x11grab);
    REGISTER_INDEV   (X11GRAB_XCB,      x11grab_xcb);
    REGISTER_OUTDEV  (XV,               xv);
    
    REGISTER_INDEV   (LIBCDIO,          libcdio);
    REGISTER_INDEV   (LIBDC1394,        libdc1394);
}

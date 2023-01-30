};
static int ConvertKey(SDLKey sdl_key)
{
    for (int i = 0; sdlkeys_to_vlckeys[i].sdl_key != 0; i++) {
        if (sdlkeys_to_vlckeys[i].sdl_key == sdl_key)
            return sdlkeys_to_vlckeys[i].vlckey;
    }
    return 0;
}

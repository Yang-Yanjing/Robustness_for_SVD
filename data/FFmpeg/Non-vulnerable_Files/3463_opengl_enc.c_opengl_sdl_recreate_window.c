static int opengl_sdl_recreate_window(OpenGLContext *opengl, int width, int height)
{
    opengl->surface = SDL_SetVideoMode(width, height,
                                       32, SDL_OPENGL | SDL_RESIZABLE);
    if (!opengl->surface) {
        av_log(opengl, AV_LOG_ERROR, "Unable to set video mode: %s\n", SDL_GetError());
        return AVERROR_EXTERNAL;
    }
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    return 0;
}

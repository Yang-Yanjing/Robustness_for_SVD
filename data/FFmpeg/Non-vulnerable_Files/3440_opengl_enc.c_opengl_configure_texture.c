static av_cold int opengl_configure_texture(OpenGLContext *opengl, GLuint texture,
                                            GLsizei width, GLsizei height)
{
    if (texture) {
        int new_width, new_height;
        opengl_get_texture_size(opengl, width, height, &new_width, &new_height);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, opengl->format, new_width, new_height, 0,
                     opengl->format, opengl->type, NULL);
        OPENGL_ERROR_CHECK(NULL);
    }
    return 0;
  fail:
    return AVERROR_EXTERNAL;
}

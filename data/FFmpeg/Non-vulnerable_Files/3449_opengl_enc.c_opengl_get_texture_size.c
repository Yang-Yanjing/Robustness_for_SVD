static av_cold void opengl_get_texture_size(OpenGLContext *opengl, int in_width, int in_height,
                                            int *out_width, int *out_height)
{
    if (opengl->non_pow_2_textures) {
        *out_width = in_width;
        *out_height = in_height;
    } else {
        int max = FFMIN(FFMAX(in_width, in_height), opengl->max_texture_size);
        unsigned power_of_2 = 1;
        while (power_of_2 < max)
            power_of_2 *= 2;
        *out_height = power_of_2;
        *out_width = power_of_2;
        av_log(opengl, AV_LOG_DEBUG, "Texture size calculated from %dx%d into %dx%d\n",
               in_width, in_height, *out_width, *out_height);
    }
}

static uint8_t* opengl_get_plane_pointer(OpenGLContext *opengl, AVPacket *pkt, int comp_index,
                                         const AVPixFmtDescriptor *desc)
{
    uint8_t *data = pkt->data;
    int wordsize = opengl_type_size(opengl->type);
    int width_chroma = FF_CEIL_RSHIFT(opengl->width, desc->log2_chroma_w);
    int height_chroma = FF_CEIL_RSHIFT(opengl->height, desc->log2_chroma_h);
    int plane = desc->comp[comp_index].plane;
    switch(plane) {
    case 0:
        break;
    case 1:
        data += opengl->width * opengl->height * wordsize;
        break;
    case 2:
        data += opengl->width * opengl->height * wordsize;
        data += width_chroma * height_chroma * wordsize;
        break;
    case 3:
        data += opengl->width * opengl->height * wordsize;
        data += 2 * width_chroma * height_chroma * wordsize;
        break;
    default:
        return NULL;
    }
    return data;
}

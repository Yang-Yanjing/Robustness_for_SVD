static int opengl_type_size(GLenum type)
{
    switch(type) {
    case GL_UNSIGNED_SHORT:
    case FF_GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
        return 2;
    case GL_UNSIGNED_BYTE:
    case FF_GL_UNSIGNED_BYTE_3_3_2:
    case FF_GL_UNSIGNED_BYTE_2_3_3_REV:
    default:
        break;
    }
    return 1;
}

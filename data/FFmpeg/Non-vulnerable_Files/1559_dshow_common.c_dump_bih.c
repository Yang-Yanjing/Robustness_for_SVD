static void dump_bih(void *s, BITMAPINFOHEADER *bih)
{
    dshowdebug("      BITMAPINFOHEADER\n");
    dstruct(s, bih, biSize, "lu");
    dstruct(s, bih, biWidth, "ld");
    dstruct(s, bih, biHeight, "ld");
    dstruct(s, bih, biPlanes, "d");
    dstruct(s, bih, biBitCount, "d");
    dstruct(s, bih, biCompression, "lu");
    dshowdebug("      biCompression:\t\"%.4s\"\n",
                   (char*) &bih->biCompression);
    dstruct(s, bih, biSizeImage, "lu");
    dstruct(s, bih, biXPelsPerMeter, "lu");
    dstruct(s, bih, biYPelsPerMeter, "lu");
    dstruct(s, bih, biClrUsed, "lu");
    dstruct(s, bih, biClrImportant, "lu");
}

}
static offscreen_bitmap_t *Compose(int i_text_align,
                                    CFMutableAttributedStringRef p_attrString,
                                    unsigned i_width,
                                    unsigned i_height,
                                    unsigned *pi_textblock_height)
{
    offscreen_bitmap_t  *p_offScreen  = NULL;
    CGColorSpaceRef      p_colorSpace = NULL;
    CGContextRef         p_context = NULL;
    p_context = CreateOffScreenContext(i_width, i_height, &p_offScreen, &p_colorSpace);
    *pi_textblock_height = 0;
    if (p_context) {
        float horiz_flush;
        CGContextSetTextMatrix(p_context, CGAffineTransformIdentity);
        if (i_text_align == SUBPICTURE_ALIGN_RIGHT)
            horiz_flush = 1.0;
        else if (i_text_align != SUBPICTURE_ALIGN_LEFT)
            horiz_flush = 0.5;
        else
            horiz_flush = 0.0;
        // Create the framesetter with the attributed string.
        CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(p_attrString);
        if (framesetter) {
            CTFrameRef frame;
            CGMutablePathRef p_path = CGPathCreateMutable();
            CGRect p_bounds = CGRectMake((float)HORIZONTAL_MARGIN,
                                          (float)VERTICAL_MARGIN,
                                          (float)(i_width  - HORIZONTAL_MARGIN*2),
                                          (float)(i_height - VERTICAL_MARGIN  *2));
            CGPathAddRect(p_path, NULL, p_bounds);
            // Create the frame and draw it into the graphics context
            frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), p_path, NULL);
            CGPathRelease(p_path);
            // Set up black outlining of the text --
            CGContextSetRGBStrokeColor(p_context, 0, 0, 0, 0.5);
            CGContextSetTextDrawingMode(p_context, kCGTextFillStroke);
            if (frame != NULL) {
                CFArrayRef lines;
                CGPoint    penPosition;
                lines = CTFrameGetLines(frame);
                penPosition.y = i_height;
                for (int i=0; i<CFArrayGetCount(lines); i++) {
                    CGFloat  ascent, descent, leading;
                    CTLineRef line = (CTLineRef)CFArrayGetValueAtIndex(lines, i);
                    CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
                    // Set the outlining for this line to be dependant on the size of the line -
                    // make it about 5% of the ascent, with a minimum at 1.0
                    float f_thickness = ascent * 0.05;
                    CGContextSetLineWidth(p_context, ((f_thickness > 1.0) ? 1.0 : f_thickness));
                    double penOffset = CTLineGetPenOffsetForFlush(line, horiz_flush, (i_width  - HORIZONTAL_MARGIN*2));
                    penPosition.x = HORIZONTAL_MARGIN + penOffset;
                    penPosition.y -= ascent;
                    CGContextSetTextPosition(p_context, penPosition.x, penPosition.y);
                    CTLineDraw(line, p_context);
                    penPosition.y -= descent + leading;
                }
                *pi_textblock_height = i_height - penPosition.y;
                CFRelease(frame);
            }
            CFRelease(framesetter);
        }
        CGContextFlush(p_context);
        CGContextRelease(p_context);
    }
    if (p_colorSpace) CGColorSpaceRelease(p_colorSpace);
    return p_offScreen;
}

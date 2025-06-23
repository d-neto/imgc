#include "imagec.h"
#include "font.h"

void draw_text_spec(image_t source, const char* text, int x, int y, int point_size, hex_t color) {
    if (point_size <= 0) return;

    double target_h = (point_size / 72.0) * 96;
    double target_w = target_h;

    int x0 = x;
    int y0 = y;
    const char *ptxt = text;

    double ratio_w = (double)DEFAULT_FONT.w / target_w;
    double ratio_h = (double)DEFAULT_FONT.h / target_h;

    double threshold = 0.25;

    int a = (color >> 24) & 0xFF;
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    color = (a << 24) | (b << 16) | (g << 8) | (r);

    while (*ptxt) {
        char c = *ptxt;

        if (c == '\n') {
            y0 += (int)ceil(target_h);
            x0 = x;
            ptxt++;
            continue;
        }
        if (c == '\t') {
            x0 += (int)ceil(target_w) * 4;
            ptxt++;
            continue;
        }
        if (c < ' ' || c > '~') {
            ptxt++;
            continue;
        }

        const char *glyph = DEFAULT_FONT.data + (c - ' ') * DEFAULT_FONT.h;

        for (int ty = 0; ty < (int)target_h; ++ty) {
            for (int tx = 0; tx < (int)target_w; ++tx) {
                
                double start_sx = tx * ratio_w;
                double end_sx = (tx + 1) * ratio_w;
                double start_sy = ty * ratio_h;
                double end_sy = (ty + 1) * ratio_h;

                double total_pixels_in_area = 0;
                double lit_pixels_in_area = 0;

                for (int sy = (int)floor(start_sy); sy < (int)ceil(end_sy); ++sy) {
                    if (sy < 0 || sy >= DEFAULT_FONT.h) continue;

                    for (int sx = (int)floor(start_sx); sx < (int)ceil(end_sx); ++sx) {
                        if (sx < 0 || sx >= DEFAULT_FONT.w) continue;
                        
                        total_pixels_in_area++;

                        if ((glyph[sy] << sx) & 0x80) {
                            lit_pixels_in_area++;
                        }
                    }
                }

                if (total_pixels_in_area > 0 && (lit_pixels_in_area / total_pixels_in_area) > threshold) {
                    int pixel_x = x0 + tx;
                    int pixel_y = y0 + ty;
                    
                    if (pixel_x >= 0 && pixel_x < source.w && pixel_y >= 0 && pixel_y < source.h) {
                        for(int ch = 0; ch < source.channels; ++ch)
                            PXL_AT(source, pixel_x, pixel_y, ch) = (0xFF & (color >> (ch * 8)));
                    }
                }
            }
        }
        
        ptxt++;
        x0 += (int)ceil(target_w);
    }
}

void draw_txt(image_t source, int x, int y, int size, hex_t color,  char * _fmt_text, ...){
    char text[256] = {0};
    va_list args;
    va_start(args, _fmt_text);
    vsnprintf(text, sizeof(text), _fmt_text, args);
    va_end(args);  

    draw_text_spec(source, text, x, y, size, color);
}
#include "save.h"
#include "cairo.h"
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>

static cairo_status_t write_to_file(void *closure, const unsigned char *data,
                                    unsigned int length) {
    return fwrite(data, 1, length, (FILE *)closure) == length
        ? CAIRO_STATUS_SUCCESS
        : CAIRO_STATUS_WRITE_ERROR;
}

cairo_status_t save(cairo_surface_t *surface, struct app_state *app,
                    struct output_state *os) {
    if (!app || !os || !os->cairo_bg) {
        fprintf(stderr, "Missing state or data_device not ready\n");
        return CAIRO_STATUS_NULL_POINTER;
    }

    if (app->save_to_stdout) {
        return cairo_surface_write_to_png_stream(surface, write_to_file, stdout);
    }

    FILE *fp = fopen(app->f_path, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", app->f_path);
        return CAIRO_STATUS_WRITE_ERROR;
    }

    return cairo_surface_write_to_png_stream(surface, write_to_file, fp);
}

cairo_status_t crop_and_save(struct app_state *app, struct output_state *os,
                             int x1, int y1, int x2, int y2) {
    if (!app || !os || !os->cairo_bg) {
        fprintf(stderr, "Missing state or data_device not ready\n");
        return 1;
    }

    // Нормализация координат
    int nx1 = min_int(x1, x2), ny1 = min_int(y1, y2);
    int nx2 = max_int(x1, x2), ny2 = max_int(y1, y2);
    int w = nx2 - nx1, h = ny2 - ny1;

    if (w < 1 || h < 1) {
        fprintf(stderr, "Selection too small\n");
        return 1;
    }

    cairo_surface_t *cropped = cairo_image_surface_create_for_data(
        (unsigned char *)os->bg_data + ny1 * os->stride + nx1 * 4,
        CAIRO_FORMAT_ARGB32, w, h, os->stride);
    cairo_status_t st = save(cropped, app, os);
    cairo_surface_destroy(cropped);
    return st;
}

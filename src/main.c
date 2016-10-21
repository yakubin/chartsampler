// Copyright (c) 2016 Jakub Alba <jakubalba@gmail.com>
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software without
//    specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERSION_STR "0.1"

struct cmd_options {
    int width;
    int height;
    char * infname;
    char * outfname;
};

void print_usage(const char * exec) {
    fprintf(stderr,
"Usage: %s OPTION... FILE\n"
"Print the minimal amount of point cooridinates from FILE needed to produce\n"
"a chart equivalent to the chart produced with all the point coordinates from\n"
"FILE.\n"
"\n"
"Mandatory:\n"
"  -w WIDTH     width of the target chart (in px)\n"
"  -h HEIGHT    height of the target chart (in px)\n"
"  -o OUT       path to the output file\n"
"\n"
"Optional:\n"
"  --help       display this help and exit\n"
"  --version    output version information and exit\n"
"\n"
"Report bugs at: <https://github.com/yakubin/chartsampler/issues>\n", exec);
}

void print_version() {
    fprintf(stderr, "chartsampler %s\n", VERSION_STR);
}

struct cmd_options parse_cmd_options(int argc, char * argv[]) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "--version")) {
            print_version();
            exit(EXIT_SUCCESS);
        }
    }

    struct cmd_options opts = {};
    int opt;

    while ((opt = getopt(argc, argv, "w:h:o:")) != -1) {
        switch (opt) {
        case 'w':
            opts.width = atoi(optarg);
            break;
        case 'h':
            opts.height = atoi(optarg);
            break;
        case 'o':
            opts.outfname = strdup(optarg);
            break;
        default:
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc || opts.width == 0 || opts.height == 0 ||
            opts.outfname == NULL) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    opts.infname = strdup(argv[optind]);

    return opts;
}

typedef long double ldouble;

int64_t extend_to_int_frame(ldouble f) {
    return (f >= 0) ? ceil(f) : floor(f);
}

size_t get_orig_dimensions(int64_t * width, int64_t * height, FILE * fin) {
    int64_t x_min = 0, x_max = 0, y_min = 0, y_max = 0;
    size_t entriesn = 0;

    for (ldouble x = 0, y = 0; fscanf(fin, "%Lf %Lf", &x, &y) == 2; entriesn++) {
        int64_t framed_x = extend_to_int_frame(x);
        int64_t framed_y = extend_to_int_frame(y);

        if (framed_x < x_min) {
            x_min = framed_x;
        } else if (x_max < framed_x) {
            x_max = framed_x;
        }

        if (framed_y < y_min) {
            y_min = framed_y;
        } else if (y_max < framed_y) {
            y_max = framed_y;
        }
    }

    *width = x_max - x_min;
    *height = y_max - y_min;

    return entriesn;
}

struct point {
    ldouble x;
    ldouble y;
};

struct point_set {
    struct point * vec;
    size_t size;
    struct point ratio;
};

bool point_eq(struct point * lhs, struct point * rhs, struct point * ratio) {
    int64_t lhs_x = lhs->x * ratio->x;
    int64_t rhs_x = rhs->x * ratio->x;
    int64_t lhs_y = lhs->y * ratio->y;
    int64_t rhs_y = rhs->y * ratio->y;

    return lhs_x == rhs_x && lhs_y == rhs_y;
}

bool point_set_contains(struct point_set * set, struct point * point) {
    struct point * vec = set->vec;
    size_t size = set->size;

    for (size_t i = 0; i < size;) {
        if (point_eq(&vec[i++], point, &set->ratio)) {
            return true;
        }
    }

    return false;
}

void fill_set(struct point_set * set, FILE * fin) {
    struct point point = {};
    while (fscanf(fin, "%Lf %Lf", &point.x, &point.y) == 2) {
        if (point_set_contains(set, &point) == false) {
            set->vec[set->size++] = point;
        }
    }
}

ldouble get_ratio(ldouble target_dim, ldouble orig_dim) {
    ldouble ratio = target_dim / orig_dim;
    return (ratio > 1) ? 1 : ratio;
}

void handle_fopen_error(FILE * fp, const char * exec, const char * src) {
    if (fp) {
       return;
    }

    fprintf(stderr, "%s: ", exec);
    perror(src);
    exit(EXIT_FAILURE);
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int main(int argc, char * argv[]) {
    struct cmd_options opts = parse_cmd_options(argc, argv);

    FILE * fin = fopen(opts.infname, "r");
    handle_fopen_error(fin, argv[0], "error opening input file");
    FILE * fout = fopen(opts.outfname, "w");
    handle_fopen_error(fout, argv[0], "error opening output file");

    int64_t orig_width = 0, orig_height = 0;
    size_t entriesn = get_orig_dimensions(&orig_width, &orig_height, fin);

    fseek(fin, 0, SEEK_SET);

    struct point_set set = {};

    set.ratio.x = get_ratio(opts.width, orig_width);
    set.ratio.y = get_ratio(opts.height, orig_height);

    size_t alloc_size = MAX(set.ratio.x, set.ratio.y) * entriesn;

    set.vec = calloc(alloc_size, sizeof *set.vec);
    if (set.vec == NULL) {
        fprintf(stderr, "%s: not enough memory\n", argv[0]);
        return EXIT_FAILURE;
    }

    fill_set(&set, fin);
    fclose(fin);

    for (size_t i = 0; i < set.size; i++) {
        fprintf(fout, "%.100Lg %.100Lg\n", set.vec[i].x, set.vec[i].y);
    }

    // kernel will take a better care of this:
    // free(set.vec);
    // free(opts.infname);
    // free(opts.outfname);
    // fclose(fout);
    // so don't mind the valgrind "leaks"

    return EXIT_SUCCESS;
}

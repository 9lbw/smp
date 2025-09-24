#include <errno.h>
#include <sndfile.h>
#include <sndio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_FRAMES 4096     /* frames per read/write burst             */
#define BPS        2        /* 16-bit signed little-endian PCM (s16le) */

static void
die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* Open sndio with parameters matching the current track             */
static struct sio_hdl *
open_sndio(int rate, int ch)
{
    struct sio_par par;

    struct sio_hdl *hdl = sio_open(NULL, SIO_PLAY, 0);
    if (!hdl)
        die("sio_open");

    sio_initpar(&par);
    par.bits  = 16;
    par.sig   = 1;          /* signed */
    par.le    = 1;          /* little-endian */
    par.pchan = ch;
    par.rate  = rate;

    if (!sio_setpar(hdl, &par) || !sio_getpar(hdl, &par))
        die("sio_setpar");
    if (par.bits != 16 || par.sig != 1)
        die("device does not support 16-bit signed audio");

    if (!sio_start(hdl))
        die("sio_start");

    return hdl;
}

/* Print “Artist – Title” (or fallback to filename)                   */
static void
print_tags(SNDFILE *sf, const char *path)
{
    const char *title  = sf_get_string(sf, SF_STR_TITLE);
    const char *artist = sf_get_string(sf, SF_STR_ARTIST);

    if (title || artist)
        printf("%s%s%s\n",
               artist ? artist : "",
               (artist && title) ? " – " : "",
               title ? title : "");
    else
        printf("%s\n", path);

    fflush(stdout);
}

/* Play one file; returns 0 on success, non-zero on fatal error       */
static int
play_file(const char *path)
{
    SF_INFO info   = {0};
    SNDFILE *sf    = sf_open(path, SFM_READ, &info);
    if (!sf) {
        fprintf(stderr, "%s: %s\n", path, sf_strerror(NULL));
        return 1;
    }

    /* Print metadata                                                        */
    print_tags(sf, path);

    /* Prepare sndio                                                         */
    struct sio_hdl *hdl = open_sndio(info.samplerate, info.channels);

    /* I/O loop                                                              */
    int16_t buf[BUF_FRAMES * info.channels];
    sf_count_t frames;
    while ((frames = sf_readf_short(sf, buf, BUF_FRAMES)) > 0) {
        size_t to_write = (size_t)frames * info.channels * BPS;
        const uint8_t *p = (const uint8_t *)buf;

        while (to_write > 0) {
            ssize_t n = sio_write(hdl, p, to_write);
            if (n == 0)
                die("sio_write");
            p += n;
            to_write -= (size_t)n;
        }
    }

    sio_close(hdl);
    sf_close(sf);
    return 0;
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s file1 [file2 …]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int rc = 0;
    for (int i = 1; i < argc; i++)
        rc |= play_file(argv[i]);

    return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}

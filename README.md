smp – tiny cli music player for openbsd
--------------------------------------

what  
    plays one file through sndio, prints “artist – title”, then quits.

needs  
    openbsd, sndio, libsndfile, pkgconf.

build  
    doas pkg_add sndio libsndfile pkgconf  
    make

run  
    ./smp song.flac

license  
    bsd 2-clause – see license file.

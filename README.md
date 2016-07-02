# chartsampler 0.1

Chartsampler reduces the data needed to create a chart according to the
dimensions of the desired chart.

If you want to produce a chart with gnuplot, but gnuplot fails, because there is
too much data and it cannot fit it into memory, you can use chartsampler to
produce a file with the minimal amount of data which is needed to produce an
equivalent chart. Input should be formated as whitespace-separated pairs of
whitespace-separated numbers ("X Y").

It works on Linux, likely on macOS, less likely on Windows. (Tested only on
Linux.)

* License: BSDv3 (see LICENSE file)
* Authors: see AUTHORS file

## How to...

### ...compile and install

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ sudo make install
```

### ...use

```
Usage: ./chartsampler OPTION... FILE
Print the minimal amount of point cooridinates from FILE needed to produce
a chart equivalent to the chart produced with all the point coordinates from
FILE.

Mandatory:
  -w WIDTH     width of the target chart (in px)
  -h HEIGHT    height of the target chart (in px)
  -o OUT       path to the output file

Optional:
  --help       display this help and exit
  --version    output version information and exit
```

### ...report bugs

[here](https://github.com/yakubin/chartsampler/issues)

## Contributing

All the sent patches and pull requests will be presumed to be licensed under
a BSDv3-compatible license.

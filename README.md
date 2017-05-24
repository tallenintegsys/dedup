# dedup
I need a utility to do file level de-duplication.
This utility will spin through a tree of directories searching for duplicate files (it checks file size and then sha512) when it finds one (or more) duplicates it hard links them, thus, reducing disc space. I need this because my rsyncs (backups e.g. rsync --link-dest=...) got out of sync once or twice and now I have a bunch of wasted space; furthermore, dupremove (the btrfs utility that does block level deduplication like say NetApp) seems to kmalloc all my memory crashing the kernel.
This is a bit of a mess right now- it's a hodgepodge of C and C++ (mostly containers) as it is partially an effort to gain as much proficiency with C++ containers as I have with Java containers. The Dia diagram is a bit of a hack too, it's half class diagram half E-R (think SQL) diagram because SQL RDBMSs (MySQL, Postgress, Oracle etc) are what I'm used to.


Compiler note: I'm using gcc built from source, I think it compiles okay on more pedestrian compilers but I'm not really checking. I figure by the time this sees the light of day gcc 8 will be more mainstream and I was toying with some experimental C++ stuff. Also I am using some C++17 (okay, technically C++1z) stuff...

here is an excerpt of gcc -v
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/home/tallen/libexec/gcc/x86_64-pc-linux-gnu/8.0.0/lto-wrapper
Target: x86_64-pc-linux-gnu
Configured with: ../gcc/configure --prefix=/home/tallen --with-gmp=/home/tallen --disable-multilib
Thread model: posix
gcc version 8.0.0 20170513 (experimental) (GCC)

It appears to be correctly identifying duplicate files (the actual linking is trivial) but I don't believe it. It's traversing like 1TB file systems way too fast--yeah I know about disc caching and it is RAID 1 but--I must be missing something and the RAID can't be helping much as I'm single threaded.

For now I've relaxed my compiler requirements as I develop with my fancy compiled compiler but I have to rebuild it on my server as thats where the 2TB filesystem (backups and some movies too) is.

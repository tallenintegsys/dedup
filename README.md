# dedup
I need a utility to do file level de-duplication.
This utility will spin through a tree of directories searching for duplicate files (it checks file size and it that matches then sha512) when it finds one (or more) duplicates it hard links them, thus, reducing disc space. I need this because my rsyncs (backups e.g. rsync --link-dest=...) got out of sync once or twice and now I have a bunch of wasted space; furthermore, dupremove (the btrfs utility that does block level deduplication like say NetApp) seems to kmalloc all my memory crashing the kernel.
This is a bit of a mess right now- it's a hodgepodge of C and C++ (mostly containers) as it is partially an effort to gain as much proficiency with C++ containers as I have with Java containers.
The Dia diagram is a bit of a hack too, it's half class diagram half E-R (think SQL) diagram because RDBMSs are what I'm used to, not containers.

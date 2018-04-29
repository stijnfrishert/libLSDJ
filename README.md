# liblsdj

[Little Sound DJ](http://littlesounddj.com) is wonderful tool that transforms your old gameboy into a music making machine. It has a thriving community of users that pushes their old hardware to its limits, in pursuit of new musical endeavours. It can however be cumbersome to manage songs and sounds outside of the gameboy.

In this light *liblsdj* is being developed, a cross-platform and fast C utility library for interacting with the LSDJ save format (.sav), song files (.lsdsng) and more. The end goal is to deliver *liblsdj* with a suite of tools for working with everything LSDJ. Currently two such tools are included: *lsdsng-export* and *lsdsng-import*.

# Tools

## lsdsng-export

*lsdsng-export* is a command-line tool for exporting songs from a .sav to .lsdsng, and querying sav formats about their song content.

    Options:
      -h [ --help ]            Help screen
      --file arg               Input save file, can be a nameless option
      --noversion              Don't add version numbers to the filename
      -f [ --folder ]          Put every lsdsng in its own folder
      -p [ --print ]           Print a list of all songs in the sav
      -d [ --decimal ]         Use decimal notation for the version number, instead of hex
      -u [ --underscore ]      Use an underscore for the special lightning bolt  character, instead of x
      -o [ --output ] arg      Output folder for the lsdsng's
      -v [ --verbose ]         Verbose output during export
      -i [ --index ] arg       Single out a given project index to export, 0 or  more
      -n [ --name ] arg        Single out a given project by name to export
      -w [ --working-memory ]  Single out the working-memory song to export

## lsdsng-import

*lsdsng-import* is a command-line tool for importing one or more songs from .lsdsng into a .sav file.

    Options:
      -h [ --help ]                  Help screen
      -f [ --file ] arg              .lsdsng file(s), 0 or more
      -o [ --output ] arg            The output file (.sav)
      -s [ --sav ] arg               A sav file to append all .lsdsng's to
      -v [ --verbose ]               Verbose output during import

# System Requirements

The nature of *liblsdj* as a C library makes it compilable on nearly all common OSes. Both tools included have been tested on macOS Sierra and Windows 7/10 and seem to be working.

# Download

Precompiled binaries can be found under [releases](https://github.com/stijnfrishert/liblsdj/releases).

# Help out?

If you'd like to help out, let me know. It'd be a great help if more people would test the tools coming with liblsdj. Though I've tagged the first release as 1.0.0, the software is very much still in alpha stage.

Bug reports can be filed in [issues](https://github.com/stijnfrishert/liblsdj/issues). Feature requests can also be added there, appropriately labeled.

Developers that would like to help out are warmly invited to do so. This project is open source for a reason; I know the chiptune scene as a loving and caring, open community, and to me this seems like a good way to give back. I personally have little experience with open sourcing code, and view this as a good opportunity to gain some.

# License

*liblsdj* and its tools are released under the liberal MIT-license.

---

Special thanks for Defense Mechanism (urbster1) for thinking along and helping out where needed.

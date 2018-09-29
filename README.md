# AsciiImageEmbedder
Embeds an image into ascii characters, or optionally hirigana or katakana if using other fonts

If using imageMagick 6 (or below), replace any occurences of #include <MagickWand/MagickWand>  replace with  #include <wand/MagickWand>

if you can build Qt projects, you can type *make QtAskii* to build a gui version. executable is produced in ./QtAskii/build subdirectory.

otherwise, you can just make the normal terminal version with make. run with the --help flag to give some indication on what to do
for basics, just supply a path(absolute or relative) to some picture as an arguement, and execute. additional options available to set font, fontSize, and various other options.




package require tclgd

set font ./Vera.ttf

GD create img 200 200
set black [img allocate_color 0 0 0]
set white [img allocate_color 255 255 255]
set blue [img allocate_color 0 0 255]
set green [img allocate_color 0 255 0]
set red [img allocate_color 255 0 0]

#img transparent 0

img text $white $font 20 0 10 30 Hey

set ofp [open image2.gif w]
fconfigure $ofp -encoding binary -translation binary
puts $ofp [img gif_data]
flush $ofp
close $ofp

set ofp [open image.gif w]
fconfigure $ofp -encoding binary -translation binary
img write_gif $ofp
flush $ofp
close $ofp


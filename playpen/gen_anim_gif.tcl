

package require tclgd
package require Tclx

#cmdtrace on

set font ./Vera.ttf

GD create img 200 200
set white [img allocate_color 255 255 255]
set black [img allocate_color 0 0 0]

img text $black $font 20 0 10 30 Hey

set ofp [open image.gif w]
fconfigure $ofp -encoding binary -translation binary
puts [fconfigure $ofp]

img gif_anim_begin $ofp 1 0

img gif_anim_add $ofp 0 0 0 50 0

GD create img2 200 200
set white [img2 allocate_color 255 255 255]
img2 copy_palette img

img2 text $black $font 20 0 10 30 DBAKER

img2 gif_anim_add $ofp 0 0 0 50 0 img

img filled_rectangle 0 0 199 199 0
img text $black $font 20 0 10 30 YOU
img gif_anim_add $ofp 0 0 0 50 0 img2

img2 filled_rectangle 0 0 199 199 0
img2 text $black $font 20 0 10 30 SUCK
img2 gif_anim_add $ofp 0 0 0 50 0 img

img filled_rectangle 0 0 199 199 0
img gif_anim_add $ofp 0 0 0 50 0 img2

img gif_anim_end $ofp

close $ofp

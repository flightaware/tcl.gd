

package require tclgd

set font ./Vera.ttf

GD create img 201 201
set black [img allocate_color 0 0 0]
set green [img allocate_color 0 255 0]

for {set x 0}  {$x <=200} {incr x 10} {
    for {set y 0} {$y <= 200} {incr y 10} {
        img line $x 0 $x 200 $green
	img line 0 $y 200 $y $green
    }
}

img filled_rectangle 0 0 10 10 $green

set ofp [open grid.gif w]
fconfigure $ofp -encoding binary -translation binary
img write_gif $ofp
flush $ofp
close $ofp


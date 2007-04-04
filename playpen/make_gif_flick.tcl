

package require tclgd
package require Tclx

#cmdtrace on

proc doit {argv} {
    set files $argv

    set firstFile [lindex $files 0]

    set fp [open $firstFile]
    fconfigure $fp -encoding binary -translation binary
    set prev [GD create_from_gif #auto $fp]
    close $fp

    set ofp [open anim.gif w]
    fconfigure $ofp -encoding binary -translation binary
    puts [fconfigure $ofp]

    #$prev transparent 0

    $prev gif_anim_begin $ofp 1 0

    $prev gif_anim_add $ofp 0 0 0 100 1

    set delay 200

    foreach imageFile [lrange $files 1 end] {
	set fp [open $imageFile]
	fconfigure $fp -encoding binary -translation binary
	set cur [GD create_from_gif #auto $fp]
	puts "height [$cur height] width [$cur width] colors [$cur total_colors]"
	close $fp

	#$cur transparent 0

	$cur copy_palette $prev

	$cur gif_anim_add $ofp 0 0 0 $delay 2 $prev

        if {$delay == 200} {
	    set delay 40
	}

	rename $prev ""
	set prev $cur
    }

    $cur gif_anim_end $ofp
    close $ofp
}

if !$tcl_interactive {doit $argv}

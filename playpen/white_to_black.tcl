#
# white_to_black - program to convert all white pixels in a PNG image
#   to black ones.
#

package require tclgd


proc load_image {file} {
    set fp [open $file]
    fconfigure $fp -translation binary -encoding binary
    set gd [GD create_from_png #auto $fp]
    close $fp
    return $gd
}

proc store_image {gdObject file} {
    set fp [open $file w]
    fconfigure $fp -translation binary -encoding binary
    $gdObject write_png $fp 9
    close $fp
    return $gdObject
}

proc rewrite_colors {gd r1 g1 b1 r2 g2 b2} {
    set sourceColor [$gd exact_color $r1 $g1 $b1]
    set destColor [$gd exact_color $r2 $g2 $b2]

    set nChanged [$gd rewrite_color $sourceColor $destColor]
    return $nChanged
}

proc usage {} {
    puts stderr "usage: $::argv0 sourceImageFile destImageFile"
    exit 1
}

proc main {{argv ""}} {
    if {[llength $argv] != 2} {
        usage
    }
    lassign $argv sourceFile destFile

    set gd [load_image $sourceFile]
    set nChanged [rewrite_colors $gd 255 255 255 0 0 0]
    store_image $gd $destFile
    puts "$nChanged pixels changed"
    exit 0
}

if !$tcl_interactive {
    main $argv
}



load ./libCTrackStream0.0.so

CTrackStream create x

#x update timestamp longitude latitude groundspeed altitude altitudeStatus updateType"

x update 12345 -123 234 300 350 C 6

x update 12380 -124 235 301 351 C 6

x update 12420 -125 236 302 352 C 6

puts "dumping history via arrays"
x history -array data {
    parray data
    puts ""
}
puts ""

puts "dumping history via lists"
x history -list mylist {
    puts $mylist
}
puts ""

memory onexit /tmp/urk
exit

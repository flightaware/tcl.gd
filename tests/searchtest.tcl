


load ./libCTrackStream2.0.so

CTrackStream create u
CTrackStream create v
CTrackStream create w
CTrackStream create x
CTrackStream create y
CTrackStream create z

u _update 12345 3000 1 300 300 C 6

v _update 12345 1 3000 300 300 C 6

w _update 12345 1 1211 300 300 C 6
w arrived

x departed

y _update 12345 1 -1211 300 300 C 6

z _update 12345 -1230 1130 300 300 C 6

#puts [search_tracks latlong -2000 -2000 2000 2000]

puts [search_tracks latlong -2000 -2000 2000 2000]


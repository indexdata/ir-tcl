# $Id: iterate.tcl,v 1.1 1995-03-20 08:53:28 adam Exp $
#
# Small test script which searches for adam ...
proc init-response {} {
    global count

    set count 0
    puts "In init-response"
    z callback {search-response}
    z.1 search adam
}

proc search-response {} {
    global count

    incr count
    puts "In search-response $count"
    z callback {search-response}
    z.1 search adam
}

ir z
ir-set z.1
z databaseNames DEM
z connect localhost:9999
z callback {init-response}
z init


# $Id: iterate.tcl,v 1.2 1995-05-26 11:44:10 adam Exp $
#
# Small test script which searches for science ...
proc init-response {} {
    global count

    set count 0
    puts "In init-response"
    do-search
}

proc do-search {} {
    global count

    incr count
    puts $count
    z callback {search-response}
    z.1 search science
}

proc search-response {} {
    set hits [z.1 resultCount]
    if {$hits <= 0} {
        do-search
    }
    z callback {present-response}
    z.1 present 1 $hits
}

proc present-response {} {
    do-search
}

ir z
z databaseNames DEM
z connect localhost:9999
z callback {init-response}
ir-set z.1 z
z init


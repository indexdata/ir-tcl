# $Id: iterate.tcl,v 1.5 1996-08-21 11:24:01 adam Exp $
#
# Small test script which searches for science ...
proc fail-back {} {
    puts "Fail"
}

proc connect-response {} {
    z callback {init-response}
    ir-set z.1 z
    z init
}
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
    if {$hits < 10} {
        z.1 present 1 $hits
    } else {
        z.1 present 1 10
    }
}

proc present-response {} {
    do-search
}

ir z
z failback {fail-back}
z databaseNames dummy
z callback {connect-response}
z connect localhost:9999
vwait forever

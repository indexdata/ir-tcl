# $Id: iterate.tcl,v 1.6 1997-04-13 19:00:43 adam Exp $
#
# Small test script which searches for science ...
proc fail-back {} {
    puts "Fail"
}

proc connect-response {} {
    z callback {init-response}
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
    catch {rename z.1 {}}
    ir-set z.1 z
    z callback {search-response}
    z.1 preferredRecordSyntax sutrs
    z.1 search the
}

proc search-response {} {
    set hits [z.1 resultCount]
    if {$hits <= 0} {
        do-search
        return
    }
    z callback {present-response}
    if {$hits < 20} {
        z.1 present 1 $hits
    } else {
        z.1 present 1 20
    }
}

proc present-response {} {
    do-search
}

ir z
z failback {fail-back}
z databaseNames Default
z callback {connect-response}
z connect localhost:9999
vwait forever

# $Id: shell.tcl,v 1.3 1996-08-21 11:24:03 adam Exp $
#
source display.tcl

ir z
set pref(base) Default
set pref(format) usmarc

proc help {} {
    puts "Commands:"
    puts " target <host>"
    puts " base <base>"
    puts " format <format>"
    puts " find <query>"
    puts " show <offset> <number>"
    puts ""
}

proc fail-response {} {
    global ok
    set ok -1
}
proc target {name} {
    global ok pref

    set ok 0
    z disconnect
    z failback {fail-response}
    z callback {connect-response}
    if [catch "z connect $name"] {
        fail-response
    } else {
        vwait ok
    }
    return {}
}

proc base {base} {
    global pref
    set pref(base) $base
}

proc format {format} {
    global pref
    set pref(format) $format
}

proc connect-response {} {
    z callback {init-response}
    z init
}

proc init-response {} {
    global ok pref

    set ok 1
    puts "Connected and initialized."
    ir-set z.1 z
}

proc find-response {z} {
    set sstatus [$z searchStatus]
    if {$sstatus} {
        set h [$z resultCount]
        puts "Search ok. $h hits"
    } else {
        puts "Search failed"
    }
    common-response $z 1
}

proc common-response {z from} {
    global ok pref

    set ok 1
    set status [$z responseStatus]
    switch [lindex $status 0] {
    NSD { 
            puts -nonewline "NSD"
            puts -nonewline [lindex $status 1]
            puts -nonewline " "
            puts -nonewline [lindex $status 2]
            puts -nonewline ": "
            puts -nonewline [lindex $status 3]
            puts ""
        }
    DBOSD {
            puts "DBOSD"
            for {set i $from} {$i < [$z nextResultSetPosition]} {incr i} {
                if {[$z type $i] == ""} {
                    break
                }
                puts "\# $i"
		display $z $i
            }
        }
    }
}

proc show {from number} {
    global ok pref

    set ok 0
    z callback "common-response z.1 $from"
    z.1 present $from $number
    vwait ok
    return {}
}
    
proc find {query} {
    global ok pref

    set ok 0
    z.1 databaseNames $pref(base)
    z.1 preferredRecordSyntax $pref(format)
    z callback {find-response z.1}
    z.1 search $query
    vwait ok
    return {}
}


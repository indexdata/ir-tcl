# $Id: shell.tcl,v 1.8 2002-03-21 11:11:53 adam Exp $
#

if {[catch {ir-log-init all irtcl shell.log}]} {
    set e [info sharedlibextension]
    puts "Loading irtcl$e ..."
    load ./irtcl$e irtcl
    ir-log-init all irtcl shell.log
}

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
    puts " sort <attr> <flag>"
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
    } elseif {$ok == 0} {
        vwait ok
    }
    if {$ok == 1} {
        puts "Connected and initialized."
    } else {
        puts "Failed."
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
    ir-set z.1 z
}

proc find-response {z} {
    set sstatus [$z searchStatus]
    if {$sstatus} {
        set h [$z resultCount]
        puts "Search ok. $h hits"
	set terms [$z searchResult]
	foreach tc $terms {
            puts "[lindex $tc 0]: [lindex $tc 1]"
        }
    } else {
        puts "Search failed"
    }
    common-response $z 1
}

proc sort-response {z} {
    global ok
    set sstatus [$z sortStatus]
    puts "Sort Status: $sstatus"
    set ok 1
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
            set to [expr $from + [$z numberOfRecordsReturned]]
            for {set i $from} {$i < $to} {incr i} {
                if {[$z type $i] == ""} {
                    break
                }
                puts "\# $i"
		display $z $i
            }
        }
    }
}

proc show {{from 1} {number 1}} {
    global ok pref

    set ok 0
    z callback "common-response z.1 $from"
    z.1 present $from $number
    vwait ok
    return {}
}

proc explain {query} {
    global ok pref

    set ok 0
    z.1 databaseNames IR-Explain-1
    z.1 preferredRecordSyntax explain
    z callback {find-response z.1}
    z.1 search "@attrset exp1 @attr 1=1 @attr 2=3 @attr 3=3 @attr 4=3 $query"
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

proc sort {query flags} {
    global ok pref

    set ok 0
    z callback {sort-response z.1}
    z.1 sort "$query $flags"
    vwait ok
    return {}
}


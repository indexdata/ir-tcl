# $Id: shell.tcl,v 1.2 1995-08-28 12:21:22 adam Exp $
#
source display.tcl

ir z

proc help {} {
    puts "Commands:"
    puts " target <host> <database>"
    puts " find <query>"
    puts " show <offset> <number>"
    puts ""
}

proc target {name database} {
    z disconnect
    z failback {puts "Connection failed"}
    z callback {connect-response}
    z databaseNames $database
    z connect $name
    return {}
}

proc connect-response {} {
    z callback {init-response}
    z init
}

proc init-response {} {
    puts "Connect and initialized."
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
    set status [lindex [$z responseStatus] 0]
    switch $status {
    NSD { 
            puts -nonewline "NSD"
            puts -nonewline [lindex [$z responseStatus] 1]
            puts -nonewline " "
            puts -nonewline [lindex [$z responseStatus] 2]
            puts -nonewline ": "
            puts -nonewline [lindex [$z responseStatus] 3]
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
    z callback "common-response z.1 $from"
    z.1 present $from $number
}
    
proc find {query} {
    ir-set z.1 z
    z failback {puts "Connection closed"}
    z callback {find-response z.1}
    z.1 search $query
}


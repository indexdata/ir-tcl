#!/usr/bin/tclsh
# $Id: msearch.tcl,v 1.1 2001-03-26 11:39:35 adam Exp $
# Simple multi-target search

if {[catch {ir-log-init all irtcl shell.log}]} {
    set e [info sharedlibextension]
    puts "Loading irtcl$e ..."
    load ./irtcl$e irtcl
    ir-log-init all irtcl shell.log
}

proc msearch {targets query pending} {
	global $pending

	set n 0
	foreach t $targets {
		ir z.$n
		z.$n databaseNames [lindex $t 1]
		ir-set z.$n.1 z.$n
		z.$n failback [list fail-response $targets $n $query $pending]
		z.$n callback [list connect-response $targets $n $query $pending]
		incr n
	}
	set n 0
	foreach t $targets {
		if {[catch {z.$n connect [lindex $t 0]}]} {
			fail-response $targets $n
		}
		incr n
	}
	set $pending $n
}

proc fail-response {targets n query pending} {
	global $pending

	puts "[lindex $targets $n]: failed"
	incr $pending -1
}

proc connect-response {targets n query pending} {
	global $pending

	puts "[lindex $targets $n]: connect response"
	z.$n callback [list init-response $targets $n $query $pending]
	if {[catch {z.$n init}]} {
		incr $pending -1
	}
}

proc init-response {targets n query pending} {
	global $pending

	puts "[lindex $targets $n]: init response"
	if {![z.$n initResult]} {
		puts "connection rejected: [z.$n userInformationField]"
		incr $pending -1
	} else {
		z.$n callback [list search-response $targets $n $query $pending]
		if {[catch {z.$n.1 search $query}]} {
			puts "[lindex $targets $n]: bad query $query"
			incr $pending -1
		}
	}
}

proc search-response {targets n query pending} {
	global $pending

	puts "[lindex $targets $n]: search response"
	set sstatus [z.$n.1 searchStatus]
	if {$sstatus} {
		set h [z.$n.1 resultCount]
		puts "[lindex $targets $n]: search ok"
		puts "[lindex $targets $n]: $h hits"
	} else {
		puts "[lindex $targets $n]: search failed"
	}
	incr $pending -1
}

msearch {{bagel.indexdata.dk gils} {localhost:9999 Default} {z3950.bell-labs.com books}} utah ok

# This looping is optional if you're using Tk (X11)
while {$ok} {
	vwait ok
}
puts "Finished searching"

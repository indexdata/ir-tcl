# $Id: att.tcl,v 1.1 1996-11-14 17:11:39 adam Exp $
# Very simple Explain test script.
proc fail-response {} {
    global ok
    puts "fail"
    set ok -1
}

proc present-response {} {
    global ok

    puts "Got Present Response"
    set ok 1
    z callback {puts ok}
}

proc search-response {} {
    puts "Got Search Response"
    set r [z.1 resultCount]
    puts "resultCount $r"
    z callback {present-response}
    z.1 present 1 $r
}

proc init-response {} {
    global ok
    if {![z initResult]} {
        puts "Connect rejected: [z userInformationField]"
        set ok -1
        return
    }
    puts "init ok"
    set ok 1
}

proc connect-response {} {
    z callback {init-response}
    z init
}

proc connect {} {
    ir z
    z callback {connect-response}
    z failback {fail-response}
    if {1} {
	puts "Connecting to AT&T's research server"
        z connect z3950.research.att.com
    } else {
	puts "Connecting to Silverplatter's internal server"
        z connect scono.silverplatter.com:7019
	z idAuthentication indexd indexd indexd
    }
}

proc explainSearch {q} {
    global ok
    z callback {search-response}
    ir-set z.1 z
    z.1 preferredRecordSyntax explain
    z.1 databaseNames IR-Explain-1
    z.1 search "@attrset exp1 @attr 1=1 $q"
    set ok 0
    vwait ok
}

proc DatabaseInfo {} {
    explainSearch DatabaseInfo
}

proc TargetInfo {} {
    explainSearch TargetInfo
}

set ok 0
connect
vwait ok
puts "Type"
puts "  explainSearch <category>"
puts "where <category> is TargetInfo, DatabaseInfo, SchemaInfo, etc."

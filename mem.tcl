set count 0

proc fail-response {} {
    puts "Fail-response"
}

proc present-response-a {} {
    puts "present-response-a"
    z disconnect
    rename z.1 {}
    rename z {}
    start
}

proc present-response-b {} {
    puts "present-response-a"
    z disconnect
    rename z.1 {}
    z callback {connect-response}
    z connect localhost:9999
}
    
proc search-response {} {
    puts "search-response"
    set hits [z.1 resultCount]
    if {$hits > 0} {
        z callback {present-response-a}
        z.1 present 1 $hits
	return
    }
    z disconnect
    rename z.1 {}
    rename z {}
    start
}

proc init-response {} {
    puts "init-reponse"
    ir-set z.1 z
    z callback {search-response}
    z.1 search adam
}

proc connect-response {} {
    global count

    incr count
    puts $count
    puts "connect-response"
    z callback {init-response}
    z databaseNames A
    z init
}

proc start {} {
    ir z
    z comstack tcpip
    z failback {fail-response}
    z callback {connect-response}
    z connect localhost:9999
}

start
    


proc explain-search {target zz category finish response fresponse} {
    z39 callback [list explain-search-r $target $zz $category $finish \
            $response $fresponse]
    ir-set $zz z39
    $zz databaseNames IR-Explain-1
    $zz preferredRecordSyntax explain
    $zz search "@attrset exp1 @attr 1=1 $category"
}

proc explain-search-r {target zz category finish response fresponse} {
    global cancelFlag

    apduDump
    if {$cancelFlag} {
        close-target
        return
    }
    set status [$zz responseStatus]
    if {![string compare [lindex $status 0] NSD]} {
        $fresponse $target $zz $category $finish
        return
    }
    set cnt [$zz resultCount]
    if {$cnt <= 0} {
        $fresponse $target $zz $category $finish
        return
    }
    set rr [$zz numberOfRecordsReturned]
    set cnt [expr $cnt - $rr]
    if {$cnt <= 0} {
        $response $target $zz $category $finish
        return
    }
    z39 callback [list $response $target $zz $category $finish]
    incr rr
    $zz present $rr $cnt
}

proc explain-check {target finish} {
    global profile

    set time [clock seconds]
    set etime [lindex $profile($target) 19]
    if {[string length $etime]} {
        # Check last explain. If 1 day since last explain do explain egain.
        # 1 day = 86400
        if {$time > [expr 180 + $etime]} {
            explain-start $target $finish
            return
        }
    } else {
        # Check last init. If never init or 1 week after do explain anyway.
        # 1 week = 604800
        set etime [lindex $profile($target) 18]
        if {![string length $etime]} {
            explain-start $target $finish
            return
        } elseif {$time > [expr 604800 + $etime]} {
            explain-start $target $finish
            return
        }
    }
    eval $finish [list $target]
}

proc explain-start {target finish} {
    show-status Explaining 1 0
    show-message TargetInfo
    explain-search $target z39.targetInfo TargetInfo $finish \
            explain-check-1 explain-check-1f
}

proc explain-check-1f {target zz category finish} {
    eval $finish [list $target]
}

proc explain-check-1 {target zz category finish} {
    show-status Explaining 1 0
    show-message DatabaseInfo
    explain-search $target z39.databaseInfo DatabaseInfo $finish \
            explain-check-2 explain-check-1f
}

proc explain-check-2 {target zz category finish} {
    global profile settingsChanged

    set trec [z39.targetInfo getExplain 1 targetInfo]
    puts "--- targetInfo"
    puts $trec
    set no 1
    while {1} {
        if {[catch {set rec \
                [z39.databaseInfo getExplain $no databaseInfo]}]} break
        puts "--- databaseInfo $no"
        puts $rec

        lappend dbRecs $rec
        set db [lindex [lindex $rec 1] 1]
        if {![string length $db]} break
        lappend dbList $db
        incr no
    }
    if {[info exists dbList]} {
        set profile($target) [lreplace $profile($target) 7 7 $dbList]
        set profile($target) [lreplace $profile($target) 25 25 {}]
    }
    cascade-target-list
    
    set data [lindex [lindex [lindex [lindex [lindex $trec 12] 1] 1] 1] 1]
    if {[string length $data]} {
        set profile($target) [lreplace $profile($target) 0 0 $data]
    }

    set l [llength $profile($target)]
    while {$l < 29} {
        lappend profile($target) {}
        incr l
    }

    set profile($target) [lreplace $profile($target) 8 8 \
            [lindex [lindex $trec 4] 1]]
    set profile($target) [lreplace $profile($target) 19 19 \
            [clock seconds]]
    set profile($target) [lreplace $profile($target) 20 20 \
            [lindex [lindex $trec 1] 1]]
    set profile($target) [lreplace $profile($target) 21 21 \
            [lindex [lindex $trec 2] 1]]
    set profile($target) [lreplace $profile($target) 22 22 \
            [lindex [lindex $trec 6] 1]]
    set profile($target) [lreplace $profile($target) 23 23 \
            [lindex [lindex $trec 7] 1]]
    set profile($target) [lreplace $profile($target) 24 24 \
            [lindex [lindex $trec 8] 1]]
    set profile($target) [lreplace $profile($target) 26 26 \
            [lindex [lindex $trec 5] 1]]
    set profile($target) [lreplace $profile($target) 27 27 \
            [lindex [lindex [lindex [lindex [lindex $trec 10] 1] 1] 1] 1]]

    set settingsChanged 1

    eval $finish [list $target]
}

proc debug-window {} {
	set w .debug-window
    toplevel $w

    wm title $w "Debug Window" 
    
    top-down-window $w
	scrollbar $w.top.s -command [list $w.top.t yview]
    text $w.top.t -width 60 -height 10 -wrap word -relief flat -borderwidth 0 \
        -font fixed -yscroll [list $w.top.s set]
    pack $w.top.s -side right -fill y
    pack $w.top.t -expand yes -fill both -expand y
}
debug-window

#Procedure get-attributeDetails
#If the target supports explain the Attribute Details are extracted here.
#The number 1.2.840.10003.3.1 is Bib1 and 1.2.840.10003.3.2 is Gils.
proc get-attributeDetails {target base} {
	global profile
	set index 1
	if {[info commands z39.attributeDetails] == "z39.attributeDetails"} {
		foreach arrayname [array names profile] {
			if {[string first $target,AttributeDetails, $arrayname ] != -1} {
				unset profile($arrayname)
			}
		}
		.debug-window.top.t insert end "Explain\n"
		while {![catch {set rec [z39.attributeDetails getExplain $index attributeDetails]}]} {
			set db [lindex [lindex $rec 1] 1]
			foreach tagset [lrange [lindex $rec 2] 1 end] {
				if {[lindex [lindex $tagset 0] 1] == "1.2.840.10003.3.1"} {
					foreach attributeType [lindex $tagset 1] {
						if {[lindex [lindex $attributeType 0] 1] == 1} {
							foreach attributeValues [lrange [lindex $attributeType 2] 1 end] {
								lappend profile($target,AttributeDetails,$db,Bib1Use) [lindex [lindex [lindex $attributeValues 0] 1] 1]
							}
						}						
					}
				}
			}	
			incr index
		}
		rename z39.attributeDetails ""
	} else {
		.debug-window.top.t insert end "Ingen explain\n"
	}
}

#Procedure change-queryInfo {target base}
#The queryInfo array is set according to the attributes obtained by explain.
proc change-queryInfo {target base} {
	global queryInfo profile bib1
	foreach tag $profile($target,AttributeDetails,$base,Bib1Use) {
		if {$tag < 1037} {
			lappend tempList [list $bib1($tag) 1=$tag]
		}
	}
	set queryInfo [lreplace $queryInfo 2 2 $tempList]
}


# Procedure explain-search
#  Issue search request with explain-attribute set and specific
#  category.
proc explain-search-request {target zz category finish response fresponse} {
    z39 callback [list explain-search-response $target $zz $category $finish \
            $response $fresponse]
    ir-set $zz z39
    $zz databaseNames IR-Explain-1
    $zz preferredRecordSyntax explain
    $zz search "@attrset exp1 @attr 1=1 @attr 2=3 @attr 3=3 @attr 4=3 $category"
}

# Procedure explain-search-response
#  Deal with search response.
proc explain-search-response {target zz category finish response fresponse} {
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
        explain-present-response $target $zz $category $finish $response $fresponse
        return
    }
    z39 callback [list explain-present-response $target $zz $category $finish \
		$response $fresponse]
    incr rr
    $zz present $rr $cnt
}

# Procedure explain-present-response
#  Deal with explain present response.
proc explain-present-response {target zz category finish response fresponse} {
    global cancelFlag

    apduDump
    if {$cancelFlag} {
        close-target
        return
    }
    set cnt [$zz resultCount]
    ir-log debug "cnt=$cnt"
    for {set i 1} {$i <= $cnt} {incr i} {
		if {[string compare [$zz type $i] DB]} {
		    $fresponse $target $zz $category $finish
		    return
		}
		if {[string compare [$zz recordType $i] Explain]} {
		    $fresponse $target $zz $category $finish
		    return
		}
    }
    $response $target $zz $category $finish
}


# Procedure explain-check-0
#  Phase 0: CategoryList
proc explain-check-0 {target zz category finish} {
    show-status Explaining 1 0
    show-message CategoryList
    explain-search-request $target z39.categoryList CategoryList $finish \
		explain-check-5 explain-check-fail
}

# Procedure explain-check-5
#  TargetInfo
proc explain-check-5 {target zz category finish} {
    show-status Explaining 1 0
    show-message TargetInfo

    explain-search-request $target z39.targetInfo TargetInfo $finish \
		explain-check-10 explain-check-fail
}

# Procedure explain-check-10
#  DatabaseInfo
proc explain-check-10 {target zz category finish} {
    show-status Explaining 1 0
    show-message DatabaseInfo
    explain-search-request $target z39.databaseInfo DatabaseInfo \
		$finish explain-check-15 explain-check-fail
}

# Procedure explain-check-15
#  AttributeDetails
proc explain-check-15 {target zz category finish} {
    show-status Explaining 1 0
    show-message AttributeDetails
    explain-search-request $target z39.attributeDetails AttributeDetails \
		$finish explain-check-ok explain-check-ok
}

# Proedure explain-check-fail
#  Deal with explain check failure - call finish handler
proc explain-check-fail {target zz category finish} {
    eval $finish [list $target]
}

proc prettyDump {x} {
    foreach y $x {
		prettyDumpR $y 0
    }
}

proc prettyDumpR {x ind} {
    for {set i 0} {$i < $ind} {incr i} {
		puts -nonewline " "
    }
    set i 0
    foreach y $x {
		if {$i == 0} {
		    if {![string compare $y text]} {
				puts $x
				return
		    }
		    puts $y
		} else {
		    prettyDumpR $y [expr $ind + 2]
		}
		incr i
    }
}

# Procedure explain-check-ok
proc explain-check-ok {target zz category finish} {
    global profile settingsChanged currentDb

    puts ""
    puts ""
    puts ""
    puts ""
    set crec [z39.categoryList getExplain 1 categoryList]
    puts "--- categoryList"
    puts $crec

    set rec [z39.targetInfo getExplain 1]
    set trec [z39.targetInfo getExplain 1 targetInfo]
    puts "--- targetInfo"
    puts $rec

    set no 1
    while {1} {
        if {
        	[catch {set rec [z39.databaseInfo getExplain $no databaseInfo]}]
        } break
    	puts "--- databaseInfo $no"
		puts $rec
    	lappend dbRecs $rec
    	set db [lindex [lindex $rec 1] 1]
		if {![string length $db]} break
    	lappend dbList $db
    	incr no
    }
    if {[info exists dbList]} {
        set profile($target,databases) $dbList
    }
    cascade-target-list

    set no 1
    while {1} {
        if {
        	[catch {set rec [z39.attributeDetails getExplain $no attributeDetails]}]
		} break
        puts "--- attributeDetails $no"
		puts $rec
        incr no
    }
    set data [lindex [lindex [lindex [lindex [lindex $trec 12] 1] 1] 1] 1]
    if {[string length $data]} {
        set profile($target,descripton) $data
    }

    set profile($target,namedResultSets) [lindex [lindex $trec 4] 1]
    set profile($target,timeLastExplain) [clock seconds]
    set profile($target,targetInfoName) [lindex [lindex $trec 1] 1]
    set profile($target,recentNews) [lindex [lindex $trec 2] 1]
    set profile($target,maxResultSets) [lindex [lindex $trec 6] 1]
    set profile($target,maxResultSize) [lindex [lindex $trec 7] 1]
    set profile($target,maxTerms) [lindex [lindex $trec 8] 1]
    set profile($target,multipleDatabases) [lindex [lindex $trec 5] 1]
    set profile($target,welcomeMessage) \
		[lindex [lindex [lindex [lindex [lindex $trec 10] 1] 1] 1] 1]
    
    set settingsChanged 1
    get-attributeDetails $target $currentDb

    eval $finish [list $target]
}

# Procedure explain-refresh
proc explain-refresh {target finish} {
    explain-check-0 $target {} {} $finish
}

# Procedure explain-check
#   Checks target for explain database.
#   Evals "$finish $target" on finish.
proc explain-check {target finish base} {
    global profile
    
    set refresh 0
    set time [clock seconds]
    set etime $profile($target,timeLastExplain)
    if {[string length $etime]} {
        # Check last explain. If 1 day since last explain do explain again.
        # 1 day = 86400
        if {$time > [expr 0 + $etime]} {
	    	set refresh 1
        }
    } else {
        # Check last init. If never init or 1 week after do explain anyway.
        # 1 week = 604800
        set etime $profile($target,timeLastInit)
        if {![string length $etime]} {
	    	set refresh 1
        } elseif {$time > [expr 604800 + $etime]} {
	    	set refresh 1
        }
    }
    if {$refresh} {
		explain-refresh $target $finish
    } else {
		eval $finish [list $target]
    }
}

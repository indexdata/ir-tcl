# $Id: display.tcl,v 1.4 1998-05-20 12:27:42 adam Exp $
#
# Record display
proc display {zset no} {
    set type [$zset type $no]
    if {$type == "SD"} {
        set err [lindex [$zset diag $no] 1]
        set add [lindex [$zset diag $no] 2]
        if {$add != {}} {
            set add " :${add}"
        }
        puts "Error ${err}${add}"
        return
    }
    if {$type != "DB"} {
        return
    }
    set rtype [$zset recordType $no]
    if {$rtype == "SUTRS"} {
        puts [join [$zset getSutrs $no]]
        return
    }
    if {$rtype == "GRS-1"} {
        set r [$zset getGrs $no]
        puts $r
        return
    }
    if {$rtype == "Explain"} {
        set r [$zset getExplain $no]
        puts $r
        return
    }
    if {[catch {set r [$zset getMarc $no line * * *]}]} {
        puts "Unknown record type: $rtype"
        return
    }
    foreach line $r {
        set tag [lindex $line 0]
        set indicator [lindex $line 1]
        set fields [lindex $line 2]
        puts -nonewline "$tag "
        if {$indicator != ""} {
            puts -nonewline $indicator
        }
        foreach field $fields {
            set id [lindex $field 0]
            set data [lindex $field 1]
            if {$id != ""} {
                puts -nonewline " \$$id "
            }
            puts -nonewline $data
        }
        puts ""
    }
}


# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: raw.tcl,v $
# Revision 1.10  1996-01-23 15:24:24  adam
# Wrore more comments.
#
# Revision 1.9  1995/10/17  14:18:10  adam
# Minor changes in presentation formats.
#
# Revision 1.8  1995/10/17  10:58:09  adam
# More work on presentation formats.
#
# Revision 1.7  1995/10/12  14:46:58  adam
# The record position in the raw format is much more visible.
#
# Revision 1.6  1995/09/20  11:37:07  adam
# Work on GRS.
#
# Revision 1.5  1995/08/28  12:22:09  adam
# Use 'line' instead of 'list' in MARC extraction.
#
# Revision 1.4  1995/06/22  13:16:29  adam
# Feature: SUTRS. Setting getSutrs implemented.
# Work on display formats.
#
# Revision 1.3  1995/06/14  12:16:42  adam
# Minor presentation format changes.
#
# Revision 1.2  1995/06/12  15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#
proc display-grs-raw {w r i} {
    foreach e $r {
        for {set j 0} {$j < $i} {incr j} {
            insertWithTags $w "  " {}
        }
        insertWithTags $w "([lindex $e 0]:[lindex $e 2])" marc-tag
        if {[lindex $e 3] == "string"} {
            insertWithTags $w [lindex $e 4] {}
            insertWithTags $w "\n" {}
        } elseif {[lindex $e 3] == "subtree"} {
            insertWithTags $w "\n" {}
            display-grs-raw $w [lindex $e 4] [expr $i+1]
        } else {
            insertWithTags $w [lindex $e 4] {}
            insertWithTags $w " ?\n" {}
        }
    }
}

# Procedure display-raw {sno no w flag}
#  sno    result set number (integer)
#  no     record position (integer)
#  w      text widget in which the record should be displayed.
#  hflag  header flag. If true a header showing the record position
#         should be displayed.
# This procedure attempts to display records in a raw format.
proc display-raw {sno no w hflag} {
    if {$hflag} {
        insertWithTags $w " $no " marc-head
        insertWithTags $w "\n"
    } else {
        $w delete 0.0 end
    }
    set type [z39.$sno type $no]
    if {$type == "SD"} {
        set err [lindex [z39.$sno diag $no] 1]
        set add [lindex [z39.$sno diag $no] 2]
        if {$add != {}} {
            set add " :${add}"
        }
        insertWithTags $w "Error ${err}${add}\n" marc-id
        return
    }
    if {$type != "DB"} {
        return
    }
    set rtype [z39.$sno recordType $no]
    if {$rtype == "SUTRS"} {
        insertWithTags $w [join [z39.$sno getSutrs $no]] {}
        $w insert end "\n"
        return
    } 
    if {$rtype == "GRS1"} {
        display-grs-raw $w [z39.$sno getGrs $no] 0
        return
    }
    if {[catch {set r [z39.$sno getMarc $no line * * *]}]} {
        insertWithTags $w "Unknown record type: $rtype\n" marc-id
        return
    }
    foreach line $r {
        set tag [lindex $line 0]
        set indicator [lindex $line 1]
        set fields [lindex $line 2]
        
        if {$indicator != ""} {
            insertWithTags $w "$tag $indicator" marc-tag
        } else {
            insertWithTags $w "$tag    " marc-tag
        }
        foreach field $fields {
            set id [lindex $field 0]
            set data [lindex $field 1]
            if {$id != ""} {
                insertWithTags $w " $id " marc-id
            }
            insertWithTags $w $data {}
        }
        $w insert end "\n"
    }
}


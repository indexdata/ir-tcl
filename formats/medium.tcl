# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: medium.tcl,v $
# Revision 1.15  1997-11-19 11:22:10  adam
# Object identifiers can be accessed in GRS-1 records.
#
# Revision 1.14  1996/04/12 13:45:49  adam
# Minor changes.
#
# Revision 1.13  1996/04/12  12:25:27  adam
# Modified display of GRS-1 records to include headings for standard
# tag sets.
#
# Revision 1.12  1996/03/29  16:05:36  adam
# Bug fix: GRS records wasn't recognized.
#
# Revision 1.11  1996/01/23  15:24:23  adam
# Wrore more comments.
#
# Revision 1.10  1996/01/11  09:31:05  quinn
# Small.
#
# Revision 1.9  1995/10/17  14:18:10  adam
# Minor changes in presentation formats.
#
# Revision 1.8  1995/10/17  10:58:09  adam
# More work on presentation formats.
#
# Revision 1.7  1995/10/16  17:01:03  adam
# Medium presentation format looks better.
#
# Revision 1.6  1995/09/20  11:37:06  adam
# Work on GRS.
#
# Revision 1.5  1995/06/22  13:16:29  adam
# Feature: SUTRS. Setting getSutrs implemented.
# Work on display formats.
#
# Revision 1.4  1995/06/14  12:16:42  adam
# Minor presentation format changes.
#
# Revision 1.3  1995/06/13  14:39:06  adam
# Fix: if {$var != ""} doesn't work if var is a large numerical!
# Highlight when line format is used.
#
# Revision 1.2  1995/06/12  15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#
proc display-grs-medium {w r i} {
    global tagSet
    
    foreach e $r {
        if {[tk4]} {
            set start [$w index insert]
        } else {
            for {set j 0} {$j < $i} {incr j} {
                insertWithTags $w "  " marc-tag
            }
        }
        set ttype [lindex $e 0]
        set tval [lindex $e 2]
        if {$ttype == 3} {
            insertWithTags $w "$tval " marc-pref
        } elseif {[info exists tagSet($ttype,$tval)]} {
            insertWithTags $w "$tagSet($ttype,$tval) " marc-pref
        } else {
            insertWithTags $w "($ttype,$tval) " marc-tag
        }
        if {[lindex $e 3] == "string"} {
            insertWithTags $w [lindex $e 4] marc-text
            insertWithTags $w "\n"
        } elseif {[lindex $e 3] == "subtree"} {
            insertWithTags $w "\n"
        } else {
            insertWithTags $w [lindex $e 4] {}
            insertWithTags $w " \n" {}
        }
        if {[tk4]} {
            $w tag configure indent$i \
                    -lmargin1 [expr $i * 16] \
                    -lmargin2 [expr $i * 16 + 8]
            $w tag add indent$i $start insert
        }
        if {[lindex $e 3] == "subtree"} {
            display-grs-medium $w [lindex $e 4] [expr $i+1]
        }
    }
}

# Procedure display-medium {sno no w hflag}
#  sno    result set number (integer)
#  no     record position (integer)
#  w      text widget in which the record should be displayed
#  hflag  header flag. If true a header showing the record position
#         should be displayed.
# This procedure attempts to display records in a medium-sized format.
proc display-medium {sno no w hflag} {
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
    if {$rtype == "GRS-1"} {
        display-grs-medium $w [z39.$sno getGrs $no] 0
        return
    }
    if {[catch {set i [z39.$sno getMarc $no field 245 * a]}]} {
        insertWithTags $w "Unknown record type: $rtype\n" marc-id
        return
    }
    if {[llength $i]} {
        insertWithTags $w "Title " marc-pref
        insertWithTags $w [string trimright [lindex $i 0] /] marc-text
        set i [z39.$sno getMarc $no field 245 * b]
        if {"x$i" != "x"} {
            insertWithTags $w [string trimright [lindex $i 0] /] marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 700 * a]
    if {![llength $i]} {
        set i [z39.$sno getMarc $no field 100 * a]
    }
    if {[llength $i]} {
        if {[llength $i] > 1} {
            insertWithTags $w "Authors " marc-pref
        } else {
            insertWithTags $w "Author " marc-pref
        }
        foreach x $i {
            insertWithTags $w $x marc-it
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 110 * *]
    if {[llength $i]} {
        insertWithTags $w "Co-Author " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }

    set i [z39.$sno getMarc $no field 650 * *]
    if {[llength $i]} {
        set n 0
        insertWithTags $w "Keywords " marc-pref
        foreach x $i {
            if {$n > 0} {
                $w insert end ", "
            }
            insertWithTags $w $x marc-it
            incr n
        }
        $w insert end "\n"
    }
    set i [concat [z39.$sno getMarc $no field 260 * a] \
            [z39.$sno getMarc $no field 260 * b]]
    if {[llength $i]} {
        insertWithTags $w "Publisher " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 020 * a]
    if {[llength $i]} {
        insertWithTags $w "ISBN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 022 * a]
    if {[llength $i]} {
        insertWithTags $w "ISSN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 030 * a]
    if {[llength $i]} {
        insertWithTags $w "CODEN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 015 * a]
    if {[llength $i]} {
        insertWithTags $w "Ctl number " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 010 * a]
    if {[llength $i]} {
        insertWithTags $w "LC number " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 710 * a]
    if {[llength $i]} {
        insertWithTags $w "Corporate name " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
}

# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: medium.tcl,v $
# Revision 1.9  1995-10-17 14:18:10  adam
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
    foreach e $r {
        for {set j 0} {$j < $i} {incr j} {
            insertWithTags $w "  " marc-tag
        }
        insertWithTags $w "([lindex $e 0]:[lindex $e 2]) " marc-tag
        if {[lindex $e 3] == "string"} {
            insertWithTags $w [lindex $e 4] {}
            insertWithTags $w "\n"
        } elseif {[lindex $e 3] == "subtree"} {
            insertWithTags $w "\n"
            display-grs-medium $w [lindex $e 4] [expr $i+1]
        } else {
            insertWithTags $w [lindex $e 4] {}
            insertWithTags $w " ?\n" {}
        }
    }
}

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
    if {$rtype == "GRS1"} {
        display-grs-medium $w [z39.$sno getGrs $no] 0
        return
    }
    if {[catch {set i [z39.$sno getMarc $no field 245 * a]}]} {
        insertWithTags $w "Unknown record type: $rtype\n" marc-id
        return
    }
    if {"x$i" != "x"} {
        insertWithTags $w "Title " marc-pref
        insertWithTags $w [string trimright [lindex $i 0] /] marc-text
        set i [z39.$sno getMarc $no field 245 * b]
        if {"x$i" != "x"} {
            insertWithTags $w [string trimright [lindex $i 0] /] marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 700 * a]
    if {"x$i" == "x"} {
        set i [z39.$sno getMarc $no field 100 * a]
    }
    if {"x$i" != "x"} {
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
    if {"x$i" != "x"} {
        insertWithTags $w "Co-Author " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }

    set i [z39.$sno getMarc $no field 650 * *]
    if {"x$i" != "x"} {
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
    if {"x$i" != "x"} {
        insertWithTags $w "Publisher " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 020 * a]
    if {"x$i" != "x"} {
        insertWithTags $w "ISBN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 022 * a]
    if {"x$i" != "x"} {
        insertWithTags $w "ISSN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 030 * a]
    if {"x$i" != "x"} {
        insertWithTags $w "CODEN " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 015 * a]
    if {"x$i" != "x"} {
        insertWithTags $w "Ctl number " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 010 * a]
    if {"x$i" != "x"} {
        insertWithTags $w "LC number " marc-pref
        foreach x $i {
            insertWithTags $w $x marc-text
        }
        $w insert end "\n"
    }
}

# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: line.tcl,v $
# Revision 1.13  1996-04-12 12:25:27  adam
# Modified display of GRS-1 records to include headings for standard
# tag sets.
#
# Revision 1.12  1996/03/29  16:05:36  adam
# Bug fix: GRS records wasn't recognized.
#
# Revision 1.11  1996/01/23  15:24:21  adam
# Wrore more comments.
#
# Revision 1.10  1995/10/17  17:39:46  adam
# Minor bug fix in call to display-grs-line.
#
# Revision 1.9  1995/10/17  14:18:09  adam
# Minor changes in presentation formats.
#
# Revision 1.8  1995/10/17  10:58:08  adam
# More work on presentation formats.
#
# Revision 1.7  1995/09/20  11:37:06  adam
# Work on GRS.
#
# Revision 1.6  1995/06/29  12:34:20  adam
# IrTcl now works with both tk4.0b4/tcl7.4b4 and tk3.6/tcl7.3
#
# Revision 1.5  1995/06/22  13:16:28  adam
# Feature: SUTRS. Setting getSutrs implemented.
# Work on display formats.
#
# Revision 1.4  1995/06/19  08:10:21  adam
# Inverse highligt colours in monochrome mode.
#
# Revision 1.3  1995/06/16  12:29:00  adam
# Use insertWithTags on diagnostic errors.
#
# Revision 1.2  1995/06/13  14:39:06  adam
# Fix: if {$var != ""} doesn't work if var is a large numerical!
# Highlight when line format is used.
#
# Revision 1.1  1995/06/12  15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#
proc display-grs-line {w r i} {
    global tagSet
    
    if {[tk4]} {
        set start [$w index insert]
    }
    foreach e $r {
        if {![tk4]} {
            for {set j 0} {$j < $i} {incr j} {
                insertWithTags $w "  " marc-tag
            }
        }
        set ttype [lindex $e 0]
        set tval [lindex $e 2]
        if {[info exists tagSet($ttype,$tval)]} {
            insertWithTags $w "$tagSet($ttype,$tval) " marc-pref
        } else {
            insertWithTags $w "$tval " marc-pref
        }
        if {[lindex $e 3] == "string"} {
            insertWithTags $w [lindex $e 4] marc-text
            insertWithTags $w "\n"
            break
        }
    }
    if {[tk4]} {
        $w tag configure indent$i \
                -lmargin1 [expr $i * 10] \
                -lmargin2 [expr $i * 10 + 5]
        $w tag add indent$i $start insert
    }
}

# Procedure display-line {sno no w hflag}
#  sno    result set number (integer)
#  no     record position (integer)
#  w      text widget in which the record should be displayed.
#  hflag  header flag. If true a header showing the record position
#         should be displayed.
# This procedure attempts to display records in a line-per-line format.
proc display-line {sno no w hflag} {
    global monoFlag
    set type [z39.$sno type $no] 
    if {$hflag} {
        if {! $monoFlag} {
            $w tag bind r$no <Any-Enter> \
                [list $w tag configure r$no -background gray80]
            $w tag bind r$no <Any-Leave> \
                [list $w tag configure r$no -background {}]
        } else {
            $w tag bind r$no <Any-Enter> \
                [list $w tag configure r$no -background black -foreground white]
            $w tag bind r$no <Any-Leave> \
                [list $w tag configure r$no -background {} -foreground {}]
        }
    } else {
        $w delete 0.0 end
    }
    if {$hflag} {
        set nostr [format "%5d " $no]
        insertWithTags $w $nostr marc-small-head
    }
    if {$type == "DB"} {
        set rtype [z39.$sno recordType $no]
        if {$rtype == "SUTRS"} {
            insertWithTags $w [join [z39.$sno getSutrs $no]]
        } elseif {$rtype == "GRS-1"} {
            display-grs-line $w [z39.$sno getGrs $no] 0
        } else {
            if {[catch {
                set title [lindex [z39.$sno getMarc $no field 245 * a] 0]
                set year  [lindex [z39.$sno getMarc $no field 260 * c] 0]
                insertWithTags $w "$title - " marc-text
                insertWithTags $w "$year\n" marc-it
            }]} {
                insertWithTags $w "Unknown record type: $rtype\n" marc-id
            }
        }
    } elseif {$type == "SD"} {
        set err [lindex [z39.$sno diag $no] 1]
        set add [lindex [z39.$sno diag $no] 2]
        if {$add != {}} {
            set add " :${add}"
        }
        insertWithTags $w "Error ${err}${add}\n" marc-id
    } 
}

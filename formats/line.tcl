# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: line.tcl,v $
# Revision 1.5  1995-06-22 13:16:28  adam
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

proc display-line {sno no w hflag} {
    set type [z39.$sno type $no] 
    if {$hflag} {
        if {[tk colormodel .] == "color"} {
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
        insertWithTags $w $nostr marc-tag
    }
    if {$type == "DB"} {
        set rtype [z39.$sno recordType $no]
        if {$rtype == "SUTRS"} {
            insertWithTags $w [join [z39.$sno getSutrs $no]]
        } else {
            if {[catch {
                set title [lindex [z39.$sno getMarc $no field 245 * a] 0]
                set year  [lindex [z39.$sno getMarc $no field 260 * c] 0]
                insertWithTags $w "$title - $year\n" marc-data
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

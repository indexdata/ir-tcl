# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: line.tcl,v $
# Revision 1.3  1995-06-16 12:29:00  adam
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
        $w tag bind r$no <Any-Enter> \
                [list $w tag configure r$no -background gray80]
        $w tag bind r$no <Any-Leave> \
                [list $w tag configure r$no -background {}]
    } else {
        $w delete 0.0 end
    }
    if {$type == "DB"} {
        if {$hflag} {
            set nostr [format "%5d " $no]
            insertWithTags $w $nostr marc-tag
        }
        set title [lindex [z39.$sno getMarc $no field 245 * a] 0]
        set year  [lindex [z39.$sno getMarc $no field 260 * c] 0]
        insertWithTags $w "$title - $year\n" marc-data
        $w tag bind marc-data 
    } elseif {$type == "SD"} {
        set err [lindex [z39.$sno diag $no] 1]
        set add [lindex [z39.$sno diag $no] 2]
        if {$add != {}} {
            set add " :${add}"
        }
        insertWithTags $w "Error ${err}${add}\n" marc-data
    } elseif {$type == ""} {
        return
    }
}

# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: line.tcl,v $
# Revision 1.1  1995-06-12 15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#

proc display-line {sno no w hflag} {
    set type [z39.$sno type $no]
    if {! $hflag} {
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
        $w insert end "Error ${err}${add}\n"
    } elseif {$type == ""} {
        return
    }
}

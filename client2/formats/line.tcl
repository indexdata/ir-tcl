# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: line.tcl,v $
# Revision 1.2  1999-03-17 09:24:11  perhans
# Explain now also finds Gils attributes.
# Support for search with Gils attributes added.
# Stop button added.
# The medium format extended and cleaned up (kommas between termes).
# Lots of minor but fixes.
#
# Revision 1.1  1998/09/30 10:53:54  perhans
# New client with better Explain support and nice icons.

proc display-grs-line {w r i} {
    global tagSet
    
    set head Untitled
    foreach e $r {
        set ttype [lindex $e 0]
        set tval [lindex $e 2]
	    if {$ttype == 2 && $tval == 1} {
	        if {[lindex $e 3] == "subtree"} {
		    set f [lindex $e 4]
		        foreach e $f {
		            if {[lindex $e 0] == 1 && [lindex $e 2] == 19} {
			            break
		            }
		        }
	        }
	        if {[lindex $e 3] == "string"} {
                set head [lindex $e 4]
	        }
            break
	    }
    }
    insertWithTags $w $head marc-text
    insertWithTags $w "\n"
    if {[tk4]} {
        $w tag configure indent$i -lmargin1 [expr $i * 10] \
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
    set type [z39.$sno type $no] 
    if {$hflag} {
        $w tag bind r$no <Any-Enter> \
            [list $w tag configure r$no -background black -foreground white]
        $w tag bind r$no <Any-Leave> \
            [list $w tag configure r$no -background {} -foreground {}]
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
                insertWithTags $w "$title" marc-text
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

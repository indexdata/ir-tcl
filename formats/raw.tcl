# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: raw.tcl,v $
# Revision 1.2  1995-06-12 15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#

proc display-raw {sno no w hflag} {
    if {$hflag} {
        insertWithTags $w "\n$no\n" {}
    } else {
        $w delete 0.0 end
    }
    set r [z39.$sno getMarc $no list * * *]
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
            set start [$w index insert]
            insertWithTags $w $data {}
        }
        $w insert end "\n"
    }
}


# IR toolkit for tcl/tk
# (c) Index Data 1995
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: medium.tcl,v $
# Revision 1.2  1995-06-12 15:18:10  adam
# Work on presentation formats. These are used in the main window as well
# as popup windows.
#
#

proc display-medium {sno no w hflag} {
    if {$hflag} {
        insertWithTags $w "\n$no\n" marc-data
    } else {
        $w delete 0.0 end
    }
    set i [z39.$sno getMarc $no field 245 * a]
    if {$i != ""} {
        set i [lindex $i 0]
        insertWithTags $w "Title:      " marc-tag
        insertWithTags $w $i marc-data
        set i [z39.$sno getMarc $no field 245 * b]
        if {$i != ""} {
            insertWithTags $w [lindex $i 0] marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 700 * a]
    if {$i == ""} {
        set i [z39.$sno getMarc $no field 100 * a]
    }
    if {$i != ""} {
        if {[llength $i] > 1} {
            insertWithTags $w "Authors:    " marc-tag
        } else {
            insertWithTags $w "Author:     " marc-tag
        }
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 110 * *]
    if {$i != ""} {
        insertWithTags $w "Co-Author:  " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }

    set i [z39.$sno getMarc $no field 650 * *]
    if {$i != ""} {
        set n 0
        insertWithTags $w "Keywords:   " marc-tag
        foreach x $i {
            if {$n > 0} {
                $w insert end ", "
            }
            insertWithTags $w $x marc-data
            incr n
        }
        $w insert end "\n"
    }
    set i [concat [z39.$sno getMarc $no field 260 * a] \
            [z39.$sno getMarc $no field 260 * b]]
    if {$i != ""} {
        insertWithTags $w "Publisher:  " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 020 * a]
    if {$i != ""} {
        insertWithTags $w "ISBN:       " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 022 * a]
    if {$i != ""} {
        insertWithTags $w "ISSN:       " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 030 * a]
    if {$i != ""} {
        insertWithTags $w "CODEN:      " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 015 * a]
    if {$i != ""} {
        insertWithTags $w "Ctl number: " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
    set i [z39.$sno getMarc $no field 010 * a]
    if {$i != ""} {
        insertWithTags $w "LC number:  " marc-tag
        foreach x $i {
            insertWithTags $w $x marc-data
        }
        $w insert end "\n"
    }
}

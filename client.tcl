#
# $Log: client.tcl,v $
# Revision 1.33  1995-06-09 11:17:35  adam
# Start work on geometry management.
#
# Revision 1.32  1995/06/07  09:16:37  adam
# New presentation format.
#
# Revision 1.31  1995/06/06  16:31:09  adam
# Bug fix: target names couldn't contain blanks.
# Bug fix: scan.
#
# Revision 1.30  1995/06/06  11:35:41  adam
# Work on scan. Display of old sets.
#
# Revision 1.29  1995/06/05  14:11:18  adam
# Bug fix in present-more.
#
# Revision 1.28  1995/06/02  14:52:13  adam
# Minor changes really.
#
# Revision 1.27  1995/06/02  14:29:42  adam
# Work on scan interface - up/down buttons.
#
# Revision 1.26  1995/06/01  16:36:46  adam
# About buttons. Minor bug fixes.
#
# Revision 1.25  1995/05/31  13:09:57  adam
# Client searches/presents may be interrupted.
# New moving book-logo.
#
# Revision 1.24  1995/05/31  08:36:24  adam
# Bug fix in client.tcl: didn't save options on clientrc.tcl.
# New method: referenceId. More work on scan.
#
# Revision 1.23  1995/05/29  10:33:41  adam
# README and rename of startup script.
#
# Revision 1.22  1995/05/26  11:44:09  adam
# Bugs fixed. More work on MARC utilities and queries. Test
# client is up-to-date again.
#
# Revision 1.21  1995/05/11  15:34:46  adam
# Scan request changed a bit. This version works with RLG.
#
# Revision 1.20  1995/04/21  16:31:57  adam
# New radiobutton: protocol (z39v2/SR).
#
# Revision 1.19  1995/04/18  16:11:50  adam
# First version of graphical Scan. Some work on query-by-form.
#
# Revision 1.18  1995/04/10  10:50:22  adam
# Result-set name defaults to suffix of ir-set name.
# Started working on scan. Not finished at this point.
#
# Revision 1.17  1995/03/31  09:34:57  adam
# Search-button disabled when there is no connection.
#
# Revision 1.16  1995/03/31  08:56:36  adam
# New button "Search".
#
# Revision 1.15  1995/03/28  12:45:22  adam
# New ir method failback: called on disconnect/protocol error.
# New ir set/get method: protocol: SR / Z3950.
# Simple popup and disconnect when failback is invoked.
#
# Revision 1.14  1995/03/22  16:07:55  adam
# Minor changes.
#
# Revision 1.13  1995/03/21  17:27:26  adam
# Short-hand keys in setup.
#
# Revision 1.12  1995/03/21  13:41:03  adam
# Comstack cs_create not used too often. Non-blocking connect.
#
# Revision 1.11  1995/03/21  10:39:06  adam
# Diagnostic error message displayed with tkerror.
#
# Revision 1.10  1995/03/20  15:24:06  adam
# Diagnostic records saved on searchResponse.
#
# Revision 1.9  1995/03/17  18:26:16  adam
# Non-blocking i/o used now. Database names popup as cascade items.
#
# Revision 1.8  1995/03/17  15:45:00  adam
# Improved target/database setup.
#
# Revision 1.7  1995/03/16  17:54:03  adam
# Minor changes really.
#
# Revision 1.6  1995/03/15  19:10:20  adam
# Database setup in protocol-setup (rather target setup).
#
# Revision 1.5  1995/03/15  13:59:23  adam
# Minor changes.
#
# Revision 1.4  1995/03/14  17:32:29  adam
# Presentation of full Marc record in popup window.
#
# Revision 1.3  1995/03/12  19:31:52  adam
# Pattern matching implemented when retrieving MARC records. More
# diagnostic functions.
#
# Revision 1.2  1995/03/10  18:00:15  adam
# Actual presentation in line-by-line format. RPN query support.
#
# Revision 1.1  1995/03/09  16:15:07  adam
# First presentRequest attempts. Hot-target list.
#
#
set hotTargets {}
set hotInfo {}
set busy 0

set profile(Default) {{} {} {210} {} 16384 8192 tcpip {} 1 {} {} z39v2}
set hostid Default
set settingsChanged 0
set setNo 0
set cancelFlag 0
set searchEnable 0
set fullMarcSeq 0
set displayFormat nice

set queryTypes {Simple}
set queryButtons { { {I 0} {I 1} {I 2} } }
set queryInfo { { {Title {1=4}} {Author {1=1}} \
        {Subject {1=21}} {Any {1=1016}} } }

set windowGeometry(.scan-window) {}

proc destroyG {w} {
    global windowGeometry
    set windowGeometry($w) [wm geometry $w]
    destroy $w
}

proc toplevelG {w} {
    global windowGeometry

    toplevel $w
    if {[info exists windowGeometry($w)]} {
        set g $windowGeometry($w)
        if {$g != ""} {
            wm geometry $w $g
        }
    }
}

wm minsize . 0 0

if {[file readable "clientrc.tcl"]} {
    source "clientrc.tcl"
}

set queryButtonsFind [lindex $queryButtons 0]
set queryInfoFind [lindex $queryInfo 0]

proc top-down-window {w} {
    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    
    pack  $w.top -side top -fill both -expand yes
    pack  $w.bot -fill both
}

proc top-down-ok-cancel {w ok-action g} {
    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -ipadx 2 -ipady 2 -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command ${ok-action}
    pack $w.bot.left.ok -expand yes -ipadx 2 -ipady 2 -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command [list destroy $w]
    pack $w.bot.cancel -side left -expand yes    

    if {$g} {
        grab $w
        tkwait window $w
    }
}

proc bottom-buttons {w buttonList g} {
    set i 0
    set l [llength $buttonList]

    frame $w.bot.$i -relief sunken -border 1
    pack $w.bot.$i -side left -expand yes -padx 5 -pady 5
    button $w.bot.$i.ok -text [lindex $buttonList $i] \
            -command [lindex $buttonList [expr $i+1]]
    pack $w.bot.$i.ok -expand yes -ipadx 2 -ipady 2 -padx 3 -pady 3 -side left

    incr i 2
    while {$i < $l} {
        button $w.bot.$i -text [lindex $buttonList $i] \
                -command [lindex $buttonList [expr $i+1]]
        pack $w.bot.$i -expand yes -ipadx 2 -ipady 2 -padx 3 -pady 3 -side left
        incr i 2
    }
    if {$g} {
        # Grab ...
        grab $w
        tkwait window $w
    }
}

proc cancel-operation {} {
    global cancelFlag
    global busy

    set cancelFlag 1
    if {$busy} {
        show-status Cancelled 0 {}
    }
}

proc show-target {target} {
    .bot.a.target configure -text "$target"
}

proc show-logo {v1} {
    global busy
    if {$busy != 0} {
        incr v1 -1
        if {$v1==0} {
            set v1 9
        }
        .bot.logo configure -bitmap @book${v1}
        after 140 [list show-logo $v1]
        return
    }
    while {1} {
        .bot.logo configure -bitmap @book1
        tkwait variable busy
        if {$busy} {
            show-logo 1
            return
        }
    }
}
        
proc show-status {status b sb} {
    global busy
    global searchEnable

    .bot.a.status configure -text "$status"
    if {$b == 1} {
        if {$busy == 0} {set busy 1}
    } else {
        set busy 0
    }
    if {$sb == {}} {
        return
    }
    if {$sb} {
        .top.service configure -state normal
        .mid.search configure -state normal
        .mid.scan configure -state normal
        .mid.present configure -state normal
        if {[winfo exists .scan-window]} {
            .scan-window.bot.2 configure -state normal
            .scan-window.bot.4 configure -state normal
        }
        set searchEnable 1
    } else {
        .top.service configure -state disabled
        .mid.search configure -state disabled
        .mid.scan configure -state disabled
        .mid.present configure -state disabled

        if {[winfo exists .scan-window]} {
            .scan-window.bot.2 configure -state disabled
            .scan-window.bot.4 configure -state disabled
        }
        set searchEnable 0
    }
}

proc show-message {msg} {
    .bot.a.message configure -text "$msg"
}

proc insertWithTags {w text args} {
    set start [$w index insert]
    $w insert insert $text
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start insert
    }
    foreach i $args {
        $w tag add $i $start insert
    }
}

proc about-target {} {
    set w .about-target-w

    toplevelG $w

    wm title $w "About target"
    top-down-window $w

    set i [z39 targetImplementationName]
    label $w.top.in -text "Implementation name: $i"
    set i [z39 targetImplementationId]
    label $w.top.ii -text "Implementation id: $i"
    set i [z39 targetImplementationVersion]
    label $w.top.iv -text "Implementation version: $i"
    set i [z39 options]
    label $w.top.op -text "Protocol options: $i"

    pack $w.top.in $w.top.ii $w.top.iv $w.top.op -side top -anchor nw

    bottom-buttons $w [list {Close} [list destroyG $w]] 1
}

proc about-origin {} {
    set w .about-origin-w

    toplevelG $w

    wm title $w "About IrTcl"
    place-force $w .
    top-down-window $w

    set i [z39 implementationName]
    label $w.top.in -text "Implementation name: $i"
    set i [z39 implementationId]
    label $w.top.ii -text "Implementation id: $i"

    pack $w.top.in $w.top.ii -side top -anchor nw

    bottom-buttons $w [list {Close} [list destroyG $w]] 1
}

proc display-raw {sno no w} {
    $w delete 0.0 end
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

proc display-nice {sno no w} {
    $w delete 0.0 end
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

proc show-full-marc {sno no b} {
    global fullMarcSeq
    global displayFormat

    if {[z39.$sno type $no] != "DB"} {
        return
    }
    if {$b} {
        set w .full-marc-$fullMarcSeq
        incr fullMarcSeq
    } else {
        set w .full-marc
    }
    if {[winfo exists $w]} {
        set new 0
    } else {

        if {$b} {
            toplevel $w
        } else {
            toplevelG $w
        }

        wm minsize $w 0 0
        
        frame $w.top -relief raised -border 1
        frame $w.bot -relief raised -border 1

        pack  $w.top -side top -fill both -expand yes
        pack  $w.bot -fill both

        text $w.top.record -width 60 -height 12 -wrap word \
                -yscrollcommand [list $w.top.s set]
        scrollbar $w.top.s -command [list $w.top.record yview]

        set new 1
    }
    $w.top.record tag configure marc-tag -foreground blue
    $w.top.record tag configure marc-data -foreground black
    $w.top.record tag configure marc-id -foreground red

    if {$displayFormat == "nice"} {
        display-nice $sno $no $w.top.record
    } else {
        display-raw $sno $no $w.top.record
    }
    if {$new} {
        bind $w.top.record <Return> {destroy .full-marc}
        
        pack $w.top.s -side right -fill y
        pack $w.top.record -expand yes -fill both

        bottom-buttons $w [list \
                {Close} [list destroy $w] \
                {Raw} [list display-raw $sno $no $w.top.record] \
                {Duplicate} [list show-full-marc $sno $no 1]] 0
    } else {
        $w.bot.2 configure -command [list display-raw $sno $no $w.top.record]
        $w.bot.4 configure -command [list show-full-marc $sno $no 1]
    }
}

proc update-target-hotlist {target} {
    global hotTargets

    set len [llength $hotTargets]
    if {$len > 0} {
        .top.target.m delete 6 [expr 6+[llength $hotTargets]]
    }
    set indx [lsearch $hotTargets $target]
    if {$indx >= 0} {
        set hotTargets [lreplace $hotTargets $indx $indx]
    }
    set hotTargets [linsert $hotTargets 0 $target]
    set-target-hotlist    
} 

proc set-target-hotlist {} {
    global hotTargets
    
    set i 1
    foreach target $hotTargets {
        .top.target.m add command -label "$i $target" -command \
                [list reopen-target $target {}]
        incr i
        if {$i > 8} {
             break
        }
    }
}

proc reopen-target {target base} {
    close-target
    open-target $target $base
    update-target-hotlist $target
}

proc define-target-action {} {
    global profile
    
    set target [.target-define.top.target.entry get]
    if {$target == ""} {
        return
    }
    update-target-hotlist $target
    foreach n [array names profile] {
        if {$n == $target} {
            protocol-setup $n
            return
        }
    }
    set seq [lindex $profile(Default) 12]
    puts "seq=${seq}"
    set profile($target) $profile(Default)
    set profile(Default) [lreplace $profile(Default) 12 12 [incr seq]]

    protocol-setup $target
    destroy .target-define
}

proc fail-response {target} {
    close-target
    tkerror "Target connection closed or protocol error"
}

proc connect-response {target} {
    puts "connect-response"
    show-target $target
    init-request
}

proc open-target {target base} {
    global profile
    global hostid

    z39 disconnect
    z39 comstack [lindex $profile($target) 6]
    z39 idAuthentication [lindex $profile($target) 3]
    z39 maximumRecordSize [lindex $profile($target) 4]
    z39 preferredMessageSize [lindex $profile($target) 5]
    puts -nonewline "maximumRecordSize="
    puts [z39 maximumRecordSize]
    puts -nonewline "preferredMessageSize="
    puts [z39 preferredMessageSize]
    show-status {Connecting} 0 0
    if {$base == ""} {
        z39 databaseNames [lindex [lindex $profile($target) 7] 0]
    } else {
        z39 databaseNames $base
    }
    z39 failback [list fail-response $target]
    z39 callback [list connect-response $target]
    z39 connect [lindex $profile($target) 1]:[lindex $profile($target) 2]
#    z39 options search present scan namedResultSets triggerResourceCtrl
    show-status {Connecting} 1 {}
    set hostid $target
    .top.target.m disable 0
    .top.target.m enable 1
    .top.target.m enable 2
}

proc close-target {} {
    global hostid
    global cancelFlag

    set cancelFlag 0
    set hostid Default
    z39 disconnect
    show-target {}
    show-status {Not connected} 0 0
    show-message {}
    .top.target.m disable 1
    .top.target.m disable 2
    .top.target.m enable 0
}

proc load-set-action {} {
    global setNo

    incr setNo
    ir-set z39.$setNo z39

    set fname [.load-set.top.filename.entry get]
    destroy .load-set
    if {$fname != ""} {
        show-status {Loading} 1 {}
        z39.$setNo loadFile $fname

        set no [z39.$setNo numberOfRecordsReturned]
        add-title-lines $setNo $no 1
    }
    set l [format "%-4d %7d" $setNo $no]
    .top.rset.m add command -label $l \
            -command [list add-title-lines $setNo 10000 1]
    show-status {Ready} 0 {}
}

proc load-set {} {
    set w .load-set

    set oldFocus [focus]
    toplevel $w

    place-force $w .

    top-down-window $w

    frame $w.top.filename
    
    pack $w.top.filename -side top -anchor e -pady 2
    
    entry-fields $w.top {filename} \
            {{Filename:}} \
            {load-set-action} {destroy .load-set}
    
    top-down-ok-cancel $w {load-set-action} 1
    focus $oldFocus
}

proc init-request {} {
    global setNo
    global cancelFlag

    if {$cancelFlag} {
        close-target
        return
    }
    z39 callback {init-response}
    show-status {Initializing} 1 {}
    z39 init
}

proc init-response {} {
    global cancelFlag

    if {$cancelFlag} {
        close-target
        return
    }
    show-status {Ready} 0 1
    if {![z39 initResult]} {
        set u [z39 userInformationField]
        close-target
        tkerror "Connection rejected by target: $u"
    }
}

proc search-request {} {
    global setNo
    global profile
    global hostid
    global busy
    global cancelFlag
    global searchEnable

    set target $hostid

    if {$searchEnable == 0} {
        return
    }
    set query [index-query]
    if {$query==""} {
        return
    }
    incr setNo
    ir-set z39.$setNo z39

    if {[lindex $profile($target) 10] == 1} {
        z39.$setNo setName $setNo
        puts "setName=${setNo}"
    } else {
        z39.$setNo setName Default
        puts "setName=Default"
    }
    if {[lindex $profile($target) 8] == 1} {
        z39.$setNo queryType rpn
    }
    if {[lindex $profile($target) 9] == 1} {
        z39.$setNo queryType ccl
    }
    z39 callback {search-response}
    z39.$setNo search $query
    show-status {Search} 1 0
}

proc scan-request {attr} {
    set w .scan-window

    global profile
    global hostid
    global scanView
    global scanTerm

    set target $hostid
    set scanView 0
    set scanTerm {}

    ir-scan z39.scan z39

    if {![winfo exists $w]} {
        toplevelG $w
        
        wm title $w "Scan"
        
        wm minsize $w 0 0

        top-down-window $w

        entry $w.top.entry -relief sunken 
        pack $w.top.entry -fill x -padx 4 -pady 2
        bind $w.top.entry <KeyRelease> [list scan-term-h $attr]
        if {1} {
            listbox $w.top.list -yscrollcommand [list $w.top.scroll set] \
                    -font fixed -geometry 50x14
            scrollbar $w.top.scroll -orient vertical -border 1
            pack $w.top.list -side left -fill both -expand yes
            pack $w.top.scroll -side right -fill y
            $w.top.scroll config -command [list $w.top.list yview]
        } else {
            listbox $w.top.list -font fixed -geometry 60x14
            pack $w.top.list -side left -fill both -expand yes
        }
        
        bottom-buttons $w [list {Close} [list destroyG $w] \
                {Up} [list scan-up $attr] \
                {Down} [list scan-down $attr]] 0
        bind $w.top.list <Up> [list scan-up $attr]
        bind $w.top.list <Down> [list scan-down $attr]
    }
    focus $w.top.entry
    z39 callback [list scan-response $attr 0 35]
    z39.scan numberOfTermsRequested 5
    z39.scan preferredPositionInResponse 1
    z39.scan scan "${attr} 0"
    
    show-status {Scan} 1 0
}

proc scan-term-h {attr} {
    global busy
    global scanTerm

    if {$busy} {
        return
    }
    set w .scan-window
    set nScanTerm [$w.top.entry get]
    if {$nScanTerm == $scanTerm} {
        return
    }
    set scanTerm $nScanTerm
    z39 callback [list scan-response $attr 0 35]
    z39.scan numberOfTermsRequested 5
    z39.scan preferredPositionInResponse 1
    puts "${attr} \{${scanTerm}\}"
    if {$scanTerm == ""} {
        z39.scan scan "${attr} 0"
    } else {
        z39.scan scan "${attr} \{${scanTerm}\}"
    }
    show-status {Scan} 1 0
}

proc scan-response {attr start toget} {
    global cancelFlag
    global scanTerm
    global scanView

    set w .scan-window
    puts "In scan-response"
    set m [z39.scan numberOfEntriesReturned]
    puts $m
    puts attr=$attr
    puts start=$start
    puts toget=$toget

    if {![winfo exists .scan-window]} {
        show-status {Ready} 0 1
        set cancelFlag 0
        return
    }
    set nScanTerm [$w.top.entry get]
    if {$nScanTerm != $scanTerm} {
        z39 callback [list scan-response $attr 0 35]
        z39.scan numberOfTermsRequested 5
        z39.scan preferredPositionInResponse 1
        set scanTerm $nScanTerm
        puts "${attr} \{${scanTerm}\}"
        if {$scanTerm == ""} {
            z39.scan scan "${attr} 0"
        } else {
            z39.scan scan "${attr} \{${scanTerm}\}"
        }
        show-status {Scan} 1 0
        return
    }
    set status [z39.scan scanStatus]
    if {$status == 6} {
        tkerror "Scan fail"
        show-status {Ready} 0 1
        set cancelFlag 0
        return
    }
    if {$toget < 0} {
        for {set i 0} {$i < $m} {incr i} {
            set term [lindex [z39.scan scanLine $i] 1]
            set nostr [format " %-6d" [lindex [z39.scan scanLine $i] 2]]
            $w.top.list insert $i "$nostr $term"
        }
        incr scanView $m
        $w.top.list yview $scanView
    } else {
        $w.top.list delete $start end
        for {set i 0} {$i < $m} {incr i} {
            set term [lindex [z39.scan scanLine $i] 1]
            set nostr [format " %-6d" [lindex [z39.scan scanLine $i] 2]]
            $w.top.list insert end "$nostr $term"
        }
    }
    if {$cancelFlag} {
        show-status {Ready} 0 1
        set cancelFlag 0
        return
    }
    if {$toget > 0 && $m > 1 && $m < $toget} {
        set ntoget [expr $toget - $m + 1]
        puts ntoget=$ntoget
        z39 callback [list scan-response $attr [expr $start + $m - 1] $ntoget]
        set q $term
        puts "down continue: $q"
        if {$ntoget > 10} {
            z39.scan numberOfTermsRequested 10
        } else {
            z39.scan numberOfTermsRequested $ntoget
        }
        z39.scan preferredPositionInResponse 1
        puts "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    if {$toget < 0 && $m > 1 && $m < [expr - $toget]} {
        set ntoget [expr - $toget - $m]
        puts ntoget=$ntoget
        z39 callback [list scan-response $attr 0 -$ntoget]
        set q [string range [$w.top.list get 0] 8 end]
        puts "up continue: $q"
        if {$ntoget > 10} {
            z39.scan numberOfTermsRequested 10
            z39.scan preferredPositionInResponse 11
        } else {
            z39.scan numberOfTermsRequested $ntoget
            z39.scan preferredPositionInResponse [incr ntoget]
        }
        puts "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    show-status {Ready} 0 1
}

proc scan-down {attr} {
    global scanView

    set w .scan-window
    set scanView [expr $scanView + 5]
    set s [$w.top.list size]
    if {$scanView > $s} {
        z39 callback [list scan-response $attr [expr $s - 1] 25]
        set q [string range [$w.top.list get [expr $s - 1]] 8 end]
        puts "down: $q"
        z39.scan numberOfTermsRequested 10
        z39.scan preferredPositionInResponse 1
        show-status {Scan} 1 0
        puts "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    $w.top.list yview $scanView
}

proc scan-up {attr} {
    global scanView

    set w .scan-window
    set scanView [expr $scanView - 5]
    if {$scanView < 0} {
        z39 callback [list scan-response $attr 0 -25]
        set q [string range [$w.top.list get 0] 8 end]
        puts "up: $q"
        z39.scan numberOfTermsRequested 10
        z39.scan preferredPositionInResponse 11
        show-status {Scan} 1 0
        z39.scan scan "${attr} \{$q\}"
        return
    }
    $w.top.list yview $scanView
}

proc search-response {} {
    global setNo
    global setOffset
    global setMax
    global cancelFlag
    global busy

    puts "In search-response"
    init-title-lines
    show-status {Ready} 0 1
    set setMax [z39.$setNo resultCount]
    show-message "${setMax} hits"
    set l [format "%-4d %7d" $setNo $setMax]
    .top.rset.m add command -label $l \
            -command [list add-title-lines $setNo 10000 1]
    if {$setMax <= 0} {
        set status [z39.$setNo responseStatus]
        if {[lindex $status 0] == "NSD"} {
            set code [lindex $status 1]
            set msg [lindex $status 2]
            set addinfo [lindex $status 3]
            tkerror "NSD$code: $msg: $addinfo"
        }
        return
    }
    if {$setMax > 20} {
        set setMax 20
    }
    set setOffset 1
    if {$cancelFlag} {
        set cancelFlag 0
        return
    }
    z39 callback {present-response}
    z39.$setNo present $setOffset 1
    show-status {Retrieve} 1 0
}

proc present-more {number} {
    global setNo
    global setOffset
    global setMax

    puts "setOffset=$setOffset"
    puts "present-more"
    if {$setNo == 0} {
        puts "setNo=$setNo"
	return
    }
    set max [z39.$setNo resultCount]
    if {$max <= $setOffset} {
        puts "max=$max"
        puts "setOffset=$setOffset"
        return
    }
    if {$number == ""} {
        set setMax $max
    } else {
        incr setMax $number
        if {$setMax > $max} {
            set setMax $max
        }
    }
    z39 callback {present-response}

    set toGet [expr $setMax - $setOffset + 1]
    if {$toGet <= 0} {
        return
    }
    if {$toGet > 3} {
        set toGet 3
    } 
    z39.$setNo present $setOffset $toGet
    show-status {Retrieve} 1 0
}

proc init-title-lines {} {
    .data.list delete 0 end
}

proc title-press {y setno} {
    show-full-marc $setno [expr 1 + [.data.list nearest $y]] 0
}

proc add-title-lines {setno no offset} {
    if {$offset == 1} {
        .bot.a.set configure -text $setno
        .data.list delete 0 end
    }
    bind .data.list <Double-Button-1> [list title-press %y $setno]
    bind .data.list <Button-2> [list title-press %y $setno]
    for {set i 0} {$i < $no} {incr i} {
        set o [expr $i + $offset]
        set type [z39.$setno type $o]
        if {$type == "DB"} {
            set title [lindex [z39.$setno getMarc $o field 245 * a] 0]
            set year  [lindex [z39.$setno getMarc $o field 260 * c] 0]
            set nostr [format "%5d" $o]
            .data.list insert end "$nostr $title - $year"
        } elseif {$type == "SD"} {
            set err [lindex [z39.$setno diag $o] 1]
            set add [lindex [z39.$setno diag $o] 2]
            if {$add != {}} {
                set add " :${add}"
            }
            .data.list insert end "Error ${err}${add}"
        } elseif {$type == ""} {
            break
        }
    }
}

proc present-response {} {
    global setNo
    global setOffset
    global setMax
    global cancelFlag

    puts "In present-response"
    set no [z39.$setNo numberOfRecordsReturned]
    puts "Returned $no records, setOffset $setOffset"
    add-title-lines $setNo $no $setOffset
    set setOffset [expr $setOffset + $no]
    set status [z39.$setNo responseStatus]
    if {[lindex $status 0] == "NSD"} {
        show-status {Ready} 0 1
        set code [lindex $status 1]
        set msg [lindex $status 2]
        set addinfo [lindex $status 3]
        tkerror "NSD$code: $msg: $addinfo"
        return
    }
    if {$cancelFlag} {
        show-status {Ready} 0 1
        set cancelFlag 0
        return
    }
    if {$no > 0 && $setOffset <= $setMax} {
        puts "present from ${setOffset}"
        set toGet [expr $setMax - $setOffset + 1]
        if {$toGet > 3} {
            set toGet 3
        }
        z39.$setNo present $setOffset $toGet
    } else {
        show-status {Finished} 0 1
    }
}

proc left-cursor {w} {
    set i [$w index insert]
    if {$i > 0} {
        incr i -1
        $w icursor $i
    }
}

proc right-cursor {w} {
    set i [$w index insert]
    incr i
    $w icursor $i
}

proc bind-fields {list returnAction escapeAction} {
    set max [expr [llength $list]-1]
    for {set i 0} {$i < $max} {incr i} {
        bind [lindex $list $i] <Return> $returnAction
        bind [lindex $list $i] <Escape> $escapeAction
        bind [lindex $list $i] <Tab> [list focus [lindex $list [expr $i+1]]]
        bind [lindex $list $i] <Left> [list left-cursor [lindex $list $i]]
        bind [lindex $list $i] <Right> [list right-cursor [lindex $list $i]]
    }
    bind [lindex $list $i] <Return> $returnAction
    bind [lindex $list $i] <Escape> $escapeAction
    bind [lindex $list $i] <Tab>    [list focus [lindex $list 0]]
    bind [lindex $list $i] <Left> [list left-cursor [lindex $list $i]]
    bind [lindex $list $i] <Right> [list right-cursor [lindex $list $i]]
    focus [lindex $list 0]
}

proc entry-fields {parent list tlist returnAction escapeAction} {
    set alist {}
    set i 0
    foreach field $list {
        set label ${parent}.${field}.label
        set entry ${parent}.${field}.entry
        label $label -text [lindex $tlist $i] -anchor e
        entry $entry -width 32 -relief sunken
        pack $label -side left
        pack $entry -side right
        lappend alist $entry
        incr i
    }
    bind-fields $alist $returnAction $escapeAction
}

proc define-target-dialog {} {
    set w .target-define

    toplevel $w
    place-force $w .
    top-down-window $w
    frame $w.top.target
    pack $w.top.target \
            -side top -anchor e -pady 2 
    entry-fields $w.top {target} \
            {{Target:}} \
            {define-target-action} {destroy .target-define}
    top-down-ok-cancel $w {define-target-action} 1
}

proc protocol-setup-action {target} {
    global profile
    global csRadioType
    global protocolRadioType
    global settingsChanged
    global RPNCheck
    global CCLCheck
    global ResultSetCheck

    set wno [lindex $profile($target) 12]
    set w .setup-${wno}
    
    set b {}
    set settingsChanged 1
    set len [$w.top.databases.list size]
    for {set i 0} {$i < $len} {incr i} {
        lappend b [$w.top.databases.list get $i]
    }
    set profile($target) [list [$w.top.description.entry get] \
            [$w.top.host.entry get] \
            [$w.top.port.entry get] \
            [$w.top.idAuthentication.entry get] \
            [$w.top.maximumRecordSize.entry get] \
            [$w.top.preferredMessageSize.entry get] \
            $csRadioType \
            $b \
            $RPNCheck \
            $CCLCheck \
            $ResultSetCheck \
            $protocolRadioType \
            $wno]

    cascade-target-list
    puts $profile($target)
    destroyG $w
}

proc place-force {window parent} {
    set g [wm geometry $parent]

    set p1 [string first + $g]
    set p2 [string last + $g]

    set x [expr 40+[string range $g [expr $p1 +1] [expr $p2 -1]]]
    set y [expr 60+[string range $g [expr $p2 +1] end]]
    wm geometry $window +${x}+${y}
}

proc add-database-action {target} {
    global profile

    set wno [lindex $profile($target) 12]
    set w .setup-${wno}

    $w.top.databases.list insert end \
            [.database-select.top.database.entry get]
    destroy .database-select
}

proc add-database {target} {
    global profile

    set w .database-select

    set oldFocus [focus]
    toplevel $w
 
    set wno [lindex $profile($target) 12]
    place-force $w .setup-${wno}

    top-down-window $w

    frame $w.top.database

    pack $w.top.database -side top -anchor e -pady 2
    
    entry-fields $w.top {database} \
            {{Database to add:}} \
            [list add-database-action $target] {destroy .database-select}

    top-down-ok-cancel $w [list add-database-action $target] 1
    focus $oldFocus
}

proc delete-database {target} {
    global profile

    set wno [lindex $profile($target) 12]
    set w .setup-${wno}

    foreach i [lsort -decreasing \
            [$w.top.databases.list curselection]] {
        $w.top.databases.list delete $i
    }
}

proc protocol-setup {target} {
    global profile
    global csRadioType
    global protocolRadioType
    global RPNCheck
    global CCLCheck
    global ResultSetCheck

    set wno [lindex $profile($target) 12]
    set w .setup-${wno}

    toplevelG $w

    wm title $w "Setup $target"

    top-down-window $w
    
    if {$target == ""} {
        set target Default
    }
    puts target
    puts $profile($target)

    frame $w.top.host
    frame $w.top.port
    frame $w.top.description
    frame $w.top.idAuthentication
    frame $w.top.maximumRecordSize
    frame $w.top.preferredMessageSize
    frame $w.top.cs-type -relief ridge -border 2
    frame $w.top.protocol -relief ridge -border 2
    frame $w.top.query -relief ridge -border 2
    frame $w.top.databases -relief ridge -border 2

    # Maximum/preferred/idAuth ...
    pack $w.top.description $w.top.host $w.top.port \
            $w.top.idAuthentication $w.top.maximumRecordSize \
            $w.top.preferredMessageSize -side top -anchor e -pady 2
    
    entry-fields $w.top {description host port idAuthentication \
            maximumRecordSize preferredMessageSize} \
            {{Description:} {Host:} {Port:} {Id Authentication:} \
            {Maximum Record Size:} {Preferred Message Size:}} \
            [list protocol-setup-action $target] [list destroyG $w]
    
    foreach sub {description host port idAuthentication \
            maximumRecordSize preferredMessageSize} {
        puts $sub
        bind $w.top.$sub.entry <Control-a> [list add-database $target]
        bind $w.top.$sub.entry <Control-d> [list delete-database $target]
    }
    $w.top.description.entry insert 0 [lindex $profile($target) 0]
    $w.top.host.entry insert 0 [lindex $profile($target) 1]
    $w.top.port.entry insert 0 [lindex $profile($target) 2]
    $w.top.idAuthentication.entry insert 0 [lindex $profile($target) 3]
    $w.top.maximumRecordSize.entry insert 0 [lindex $profile($target) 4]
    $w.top.preferredMessageSize.entry insert 0 [lindex $profile($target) 5]
    set csRadioType [lindex $profile($target) 6]
    set RPNCheck [lindex $profile($target) 8]
    set CCLCheck [lindex $profile($target) 9]
    set ResultSetCheck [lindex $profile($target) 10]
    set protocolRadioType [lindex $profile($target) 11]
    if {$protocolRadioType == ""} {
        set protocolRadioType z39v2
    }

    # Databases ....
    pack $w.top.databases -side left -pady 6 -padx 6 -expand yes -fill both

    label $w.top.databases.label -text "Databases"
    button $w.top.databases.add -text "Add" \
            -command [list add-database $target]
    button $w.top.databases.delete -text "Delete" \
            -command [list delete-database $target]
    listbox $w.top.databases.list -geometry 20x6 \
            -yscrollcommand "$w.top.databases.scroll set"
    scrollbar $w.top.databases.scroll -orient vertical -border 1
    pack $w.top.databases.label -side top -fill x \
            -padx 2 -pady 2
    pack $w.top.databases.add $w.top.databases.delete -side top -fill x \
            -padx 2 -pady 2
    pack $w.top.databases.list -side left -fill both -expand yes \
            -padx 2 -pady 2
    pack $w.top.databases.scroll -side right -fill y \
            -padx 2 -pady 2
    $w.top.databases.scroll config -command "$w.top.databases.list yview"

    foreach b [lindex $profile($target) 7] {
        $w.top.databases.list insert end $b
    }

    # Transport ...
    pack $w.top.cs-type -pady 6 -padx 6 -side top -fill x
    
    label $w.top.cs-type.label -text "Transport" 
    radiobutton $w.top.cs-type.tcpip -text "TCP/IP" -anchor w \
            -command {puts tcp/ip} -variable csRadioType -value tcpip
    radiobutton $w.top.cs-type.mosi -text "MOSI" -anchor w\
            -command {puts mosi} -variable csRadioType -value mosi
    
    pack $w.top.cs-type.label $w.top.cs-type.tcpip $w.top.cs-type.mosi \
            -padx 4 -side top -fill x

    # Protocol ...
    pack $w.top.protocol -pady 6 -padx 6 -side top -fill x
    
    label $w.top.protocol.label -text "Protocol" 
    radiobutton $w.top.protocol.z39v2 -text "Z39.50" -anchor w \
            -command {puts z39v2} -variable protocolRadioType -value z39v2
    radiobutton $w.top.protocol.sr -text "SR" -anchor w \
            -command {puts sr} -variable protocolRadioType -value sr
    
    pack $w.top.protocol.label $w.top.protocol.z39v2 $w.top.protocol.sr \
            -padx 4 -side top -fill x

    # Query ...
    pack $w.top.query -pady 6 -padx 6 -side top -fill x

    label $w.top.query.label -text "Query support"
    checkbutton $w.top.query.c1 -text "RPN query" -anchor w -variable RPNCheck
    checkbutton $w.top.query.c2 -text "CCL query" -anchor w -variable CCLCheck
    checkbutton $w.top.query.c3 -text "Result sets" -anchor w -variable ResultSetCheck

    pack $w.top.query.label -side top 
    pack $w.top.query.c1 $w.top.query.c2 $w.top.query.c3 \
            -padx 4 -side top -fill x

    # Ok-cancel
    bottom-buttons $w [list {Ok} [list protocol-setup-action $target] \
            {Cancel} [list destroyG $w]] 0   
#    top-down-ok-cancel $w [list protocol-setup-action $target] 0
}

proc database-select-action {} {
    set w .database-select.top
    set b {}
    foreach indx [$w.databases.list curselection] {
        lappend b [$w.databases.list get $indx]
    }
    if {$b != ""} {
        z39 databaseNames $b
    }
    destroy .database-select
}

proc database-select {} {
    set w .database-select
    global profile
    global hostid

    toplevel $w

    place-force $w .

    top-down-window $w

    frame $w.top.databases -relief ridge -border 2

    pack $w.top.databases -side left -pady 6 -padx 6 -expand yes -fill x

    label $w.top.databases.label -text "List"
    listbox $w.top.databases.list -geometry 20x6 \
            -yscrollcommand "$w.top.databases.scroll set"
    scrollbar $w.top.databases.scroll -orient vertical -border 1
    pack $w.top.databases.label -side top -fill x \
            -padx 2 -pady 2
    pack $w.top.databases.list -side left -fill both -expand yes \
            -padx 2 -pady 2
    pack $w.top.databases.scroll -side right -fill y \
            -padx 2 -pady 2
    $w.top.databases.scroll config -command "$w.top.databases.list yview"

    foreach b [lindex $profile($hostid) 7] {
        $w.top.databases.list insert end $b
    }
    top-down-ok-cancel $w {database-select-action} 1
}

proc cascade-target-list {} {
    global profile
    
    foreach sub [winfo children .top.target.m.clist] {
        puts "deleting $sub"
        destroy $sub
    }
    .top.target.m.clist delete 0 last
    foreach n [array names profile] {
        if {$n != "Default"} {
            set nl [string tolower $n]
            if {[llength [lindex $profile($n) 7]] > 1} {
                .top.target.m.clist add cascade -label $n \
                        -menu .top.target.m.clist.$nl
                menu .top.target.m.clist.$nl
                foreach b [lindex $profile($n) 7] {
                    .top.target.m.clist.$nl add command -label $b \
                            -command [list reopen-target $n $b]
                }
            } else {
                .top.target.m.clist add command -label $n \
                        -command [list reopen-target $n {}]
            }
        }
    }
    .top.target.m.slist delete 0 last
    foreach n [array names profile] {
        if {$n != "Default"} {
            .top.target.m.slist add command -label $n \
                    -command [list protocol-setup $n]
        }
    }
}

proc cascade-query-list {} {
    global queryTypes

    set i 0
    .top.options.m.slist delete 0 last
    foreach n $queryTypes {
        .top.options.m.slist add command -label $n \
                -command [list query-setup $i]
        incr i
    }

    set i 0
    .top.options.m.clist delete 0 last
    foreach n $queryTypes {
        .top.options.m.clist add command -label $n \
                -command [list query-select $i]
        incr i
    }
}

proc save-settings {} {
    global hotTargets 
    global profile
    global settingsChanged
    global queryTypes
    global queryButtons
    global queryInfo

    destroyG .

    set f [open "clientrc.tcl" w]
    puts $f "# Setup file"
    puts $f "set hotTargets \{ $hotTargets \}"

    foreach n [array names profile] {
        puts -nonewline $f "set \{profile($n)\} \{"
        puts -nonewline $f $profile($n)
        puts $f "\}"
    }
    puts -nonewline $f "set queryTypes \{" 
    puts -nonewline $f $queryTypes
    puts $f "\}"
    
    puts -nonewline $f "set queryButtons \{" 
    puts -nonewline $f $queryButtons
    puts $f "\}"
    
    puts -nonewline $f "set queryInfo \{"
    puts -nonewline $f $queryInfo
    puts $f "\}"
    
    close $f
    set settingsChanged 0
}

proc alert {ask} {
    set w .alert

    global alertAnswer

    toplevel $w
    place-force $w .
    top-down-window $w

    message $w.top.message -text $ask

    pack $w.top.message -side left -pady 6 -padx 20 -expand yes -fill x
  
    set alertAnswer 0
    top-down-ok-cancel $w {alert-action} 1
    return $alertAnswer
}

proc alert-action {} {
    global alertAnswer
    set alertAnswer 1
    destroy .alert
}

proc exit-action {} {
    global settingsChanged

    if {$settingsChanged} {
        set a [alert "you havent saved your settings. Do you wish to save?"]
        if {$a} {
            save-settings
        }
    }
    exit 0
}

proc listbuttonaction {w name h user i} {
    $w configure -text [lindex $name 0]
    $h [lindex $name 1] $user $i
}
    
proc listbuttonx {button no names handle user} {
    if {[winfo exists $button]} {
        $button configure -text [lindex [lindex $names $no] 0]
        ${button}.m delete 0 last
    } else {
        menubutton $button -text [lindex [lindex $names $no] 0] \
                -width 10 -menu ${button}.m -relief raised -border 1
        menu ${button}.m
    }
    set i 0
    foreach name $names {
        ${button}.m add command -label [lindex $name 0] \
                -command [list listbuttonaction ${button} $name \
                $handle $user $i]
        incr i
    }
}

proc listbutton {button no names} {
    menubutton $button -text [lindex $names $no] -width 10 -menu ${button}.m \
            -relief raised -border 1
    menu ${button}.m
    foreach name $names {
        ${button}.m add command -label $name \
                -command [list ${button} configure -text $name]
    }
}

proc query-add-index-action {queryNo} {
    set w .setup-query-$queryNo

    global queryInfoTmp
    global queryButtonsTmp

    lappend queryInfoTmp [list [.query-add-index.top.index.entry get] {}]

    destroy .query-add-index
    #destroy $w.top.lines
    #frame $w.top.lines -relief ridge -border 2
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
    #pack $w.top.lines -side left -pady 6 -padx 6 -fill y
}

proc query-add-line {queryNo} {
    set w .setup-query-$queryNo

    global queryInfoTmp
    global queryButtonsTmp

    lappend queryButtonsTmp {I 0}

    #destroy $w.top.lines
    #frame $w.top.lines -relief ridge -border 2
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
    #pack $w.top.lines -side left -pady 6 -padx 6 -fill y
}

proc query-del-line {queryNo} {
    set w .setup-query-$queryNo

    global queryInfoTmp
    global queryButtonsTmp

    set l [llength $queryButtonsTmp]
    if {$l <= 0} {
        return
    }
    incr l -1
    set queryButtonsTmp [lreplace $queryButtonsTmp $l $l]
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
}

proc query-add-index {queryNo} {
    set w .query-add-index

    toplevel $w
    place-force $w .setup-query-$queryNo
    top-down-window $w
    frame $w.top.index
    pack $w.top.index \
            -side top -anchor e -pady 2 
    entry-fields $w.top {index} \
            {{Index Name:}} \
            [list query-add-index-action $queryNo] {destroy .query-add-index}
    top-down-ok-cancel $w [list query-add-index-action $queryNo] 1
}

proc query-setup-action {queryNo} {
    global queryButtons
    global queryInfo
    global queryButtonsTmp
    global queryInfoTmp
    global queryButtonsFind
    global queryInfoFind

    set queryInfo [lreplace $queryInfo $queryNo $queryNo \
            $queryInfoTmp]
    set queryButtons [lreplace $queryButtons $queryNo $queryNo \
            $queryButtonsTmp]
    set queryInfoFind $queryInfoTmp
    set queryButtonsFind $queryButtonsTmp

    puts $queryInfo
    puts $queryButtons
    destroy .setup-query-$queryNo

    index-lines .lines 1 $queryButtonsFind $queryInfoFind activate-index
}

proc activate-e-index {value no i} {
    global queryButtonsTmp
    
    puts $queryButtonsTmp
    set queryButtonsTmp [lreplace $queryButtonsTmp $no $no [list I $i]]
    puts $queryButtonsTmp
    puts "value $value"
    puts "no $no"
    puts "i $i"
}

proc activate-index {value no i} {
    global queryButtonsFind

    set queryButtonsFind [lreplace $queryButtonsFind $no $no [list I $i]]

    puts "queryButtonsFind $queryButtonsFind"
    puts "value $value"
    puts "no $no"
    puts "i $i"
}

proc query-setup {queryNo} {
    set w .setup-query-$queryNo
    global queryTypes
    set queryTypes {Simple}
    global queryButtons
    global queryInfo
    global queryButtonsTmp
    global queryInfoTmp

    set queryName [lindex $queryTypes $queryNo]
    set queryInfoTmp [lindex $queryInfo $queryNo]
    set queryButtonsTmp [lindex $queryButtons $queryNo]

    #set queryButtons { {I 0 I 1 I 2} }
    #set queryInfo { { {Title ti} {Author au} {Subject sh} } }

    toplevel $w

    wm title $w "Query setup $queryName"
    place-force $w .

    top-down-window $w

    frame $w.top.lines -relief ridge -border 2
    frame $w.top.use -relief ridge -border 2
    frame $w.top.relation -relief ridge -border 2
    frame $w.top.position -relief ridge -border 2
    frame $w.top.structure -relief ridge -border 2
    frame $w.top.truncation -relief ridge -border 2
    frame $w.top.completeness -relief ridge -border 2

    # Index Lines

    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index

    pack $w.top.lines -side left -pady 6 -padx 6 -fill y

    # Use Attributes
    pack $w.top.use -side left -pady 6 -padx 6 -fill y

    label $w.top.use.label -text "Use"
    listbox $w.top.use.list -geometry 20x10 \
            -yscrollcommand "$w.top.use.scroll set"
    scrollbar $w.top.use.scroll -orient vertical -border 1
    pack $w.top.use.label -side top -fill x \
            -padx 2 -pady 2
    pack $w.top.use.list -side left -fill both -expand yes \
            -padx 2 -pady 2
    pack $w.top.use.scroll -side right -fill y \
            -padx 2 -pady 2
    $w.top.use.scroll config -command "$w.top.use.list yview"

    foreach u {{Personal name} {Corporate name}} {
        $w.top.use.list insert end $u
    }
    # Relation Attributes
    pack $w.top.relation -pady 6 -padx 6 -side top

    label $w.top.relation.label -text "Relation" -width 18
    
    listbutton $w.top.relation.b 0\
            {{None} {Less than} {Greater than or equal} \
            {Equal} {Greater than or equal} {Greater than} {Not equal} \
            {Phonetic} \
            {Stem} {Relevance} {AlwaysMatches}}
    
    pack $w.top.relation.label $w.top.relation.b -fill x 

    # Position Attributes
    pack $w.top.position -pady 6 -padx 6 -side top

    label $w.top.position.label -text "Position" -width 18

    listbutton $w.top.position.b 0 {{None} {First in field} {First in subfield}
    {Any position in field}}
    
    pack $w.top.position.label $w.top.position.b -fill x

    # Structure Attributes

    pack $w.top.structure -pady 6 -padx 6 -side top
    
    label $w.top.structure.label -text "Structure" -width 18

    listbutton $w.top.structure.b 0 {{None} {Phrase} {Word} {Key} {Year}
    {Date (norm)} {Word list} {Date (un-norm)} {Name (norm)} {Date (un-norm)}
    {Structure} {urx} {free-form} {doc-text} {local-number} {string}
    {numeric string}}

    pack $w.top.structure.label $w.top.structure.b -fill x

    # Truncation Attributes

    pack $w.top.truncation -pady 6 -padx 6 -side top
    
    label $w.top.truncation.label -text "Truncation" -width 18

    listbutton $w.top.truncation.b 0 {{Auto} {Right} {Left} {Left and right} \
            {No truncation} {Process #} {Re-1} {Re-2}}
    pack $w.top.truncation.label $w.top.truncation.b -fill x

    # Completeness Attributes

    pack $w.top.completeness -pady 6 -padx 6 -side top
    
    label $w.top.completeness.label -text "Truncation" -width 18

    listbutton $w.top.completeness.b 0 {{None} {Incomplete subfield} \
            {Complete subfield} {Complete field}}
    pack $w.top.completeness.label $w.top.completeness.b -fill x

    # Ok-cancel
    bottom-buttons $w [list \
            {Ok} [list query-setup-action $queryNo] \
            {Add index} [list query-add-index $queryNo] \
            {Add line} [list query-add-line $queryNo] \
            {Delete line} [list query-del-line $queryNo] \
            {Cancel} [list destroy $w]] 0
}

proc index-clear {} {
    global queryButtonsFind

    set i 0
    foreach b $queryButtonsFind {
        .lines.$i.e delete 0 end
        incr i
    }
}
    
proc index-query {} {
    global queryButtonsFind
    global queryInfoFind

    set i 0
    set qs {}

    foreach b $queryButtonsFind {
        set term [string trim [.lines.$i.e get]]
        if {$term != ""} {
            set attr [lindex [lindex $queryInfoFind [lindex $b 1]] 1]

            set term "\{${term}\}"
            foreach a $attr {
                set term "@attr $a ${term}"
            }
            if {$qs != ""} {
                set qs "@and ${qs} ${term}"
            } else {
                set qs $term
            }
        }
        incr i
    }
    puts "qs=  $qs"
    return $qs
}

proc index-lines {w realOp buttonInfo queryInfo handle} {
    set i 0
    foreach b $buttonInfo {
        if {! [winfo exists $w.$i]} {
            frame $w.$i -background white -border 1
        }
        listbuttonx $w.$i.l [lindex $b 1] $queryInfo $handle $i

        if {$realOp} {
            if {! [winfo exists $w.$i.e]} {
                entry $w.$i.e -width 32 -relief sunken -border 1
                bind $w.$i.e <FocusIn> [list $w.$i configure \
                        -background red]
                bind $w.$i.e <FocusOut> [list $w.$i configure \
                        -background white]
                pack $w.$i.l -side left
                pack $w.$i.e -side left -fill x -expand yes
                pack $w.$i -side top -fill x -padx 2 -pady 2
                bind $w.$i.e <Left> [list left-cursor $w.$i.e]
                bind $w.$i.e <Right> [list right-cursor $w.$i.e]
                bind $w.$i.e <Return> search-request
            }
        } else {
            pack $w.$i.l -side left
            pack $w.$i -side top -fill x -padx 2 -pady 2
        }
        incr i
    }
    set j $i
    while {[winfo exists $w.$j]} {
        destroy $w.$j
        incr j
    }
    if {! $realOp} {
        return
    }
    set j 0
    incr i -1
    while {$j < $i} {
        set k [expr $j+1]
        bind $w.$j.e <Tab> "focus $w.$k.e"
        set j $k
    }
    if {$i >= 0} {
        bind $w.$i.e <Tab> "focus $w.0.e"
        focus $w.0.e
    }
}

proc search-fields {w buttondefs} {
    set i 0
    foreach buttondef $buttondefs {
        frame $w.$i -background white
        
        listbutton $w.$i.l 0 $buttondef
        entry $w.$i.e -width 32 -relief sunken
        
        pack $w.$i.l -side left
        pack $w.$i.e -side left -fill x -expand yes

        pack $w.$i -side top -fill x -padx 2 -pady 2

        bind $w.$i.e <Left> [list left-cursor $w.$i.e]
        bind $w.$i.e <Right> [list right-cursor $w.$i.e]

        incr i
    }
    set j 0
    incr i -1
    while {$j < $i} {
        set k [expr $j+1]
        bind $w.$j.e <Tab> "focus $w.$k.e \n
        $w.$k configure -background red \n
        $w.$j configure -background white"
        set j $k
    }
    bind $w.$i.e <Tab> "focus $w.0.e \n
        $w.0 configure -background red \n
        $w.$i configure -background white"
    focus $w.0.e
    $w.0 configure -background red
}

if {[info exists windowGeometry(.w)]} {
    set g $windowGeometry(.w)
    if {$g != ""} {
        wm geometry .w $g
    }
}    

frame .top  -border 1 -relief raised
frame .lines  -border 1 -relief raised
frame .mid  -border 1 -relief raised
frame .data -border 1 -relief raised
frame .bot  -border 1 -relief raised
pack .top .lines .mid -side top -fill x
pack .data -side top -fill both -expand yes
pack .bot -fill x

menubutton .top.file -text "File" -underline 0 -menu .top.file.m
menu .top.file.m
.top.file.m add command -label "Save settings" -command {save-settings}
.top.file.m add separator
.top.file.m add command -label "Exit" -command {exit-action}
.top.file.m add separator
.top.file.m add command -label "About" -command {about-origin}

menubutton .top.target -text "Target" -underline 0 -menu .top.target.m
menu .top.target.m
.top.target.m add cascade -label "Connect" -menu .top.target.m.clist
.top.target.m add command -label "Disconnect" -command {close-target}
.top.target.m add command -label "About" -command {about-target}
.top.target.m add cascade -label "Setup" -menu .top.target.m.slist
.top.target.m add command -label "Setup new" -command {define-target-dialog}
.top.target.m add separator
set-target-hotlist

.top.target.m disable 1
.top.target.m disable 2

menu .top.target.m.clist
menu .top.target.m.slist
cascade-target-list

menubutton .top.service -text "Service" -underline 0 -menu .top.service.m
menu .top.service.m
.top.service.m add command -label "Database" -command {database-select}
.top.service.m add cascade -label "Query type" -menu .top.service.m.querytype
menu .top.service.m.querytype
.top.service.m.querytype add radiobutton -label "RPN"
.top.service.m.querytype add radiobutton -label "CCL"
.top.service.m add cascade -label "Present" -menu .top.service.m.present
menu .top.service.m.present
.top.service.m.present add command -label "More" \
        -command [list present-more 10]
.top.service.m.present add command -label "All" \
        -command [list present-more {}]
.top.service configure -state disabled

menubutton .top.rset -text "Set" -menu .top.rset.m
menu .top.rset.m
.top.rset.m add command -label "Load" -command {load-set}
.top.rset.m add separator

menubutton .top.options -text "Options" -underline 0 -menu .top.options.m
menu .top.options.m
.top.options.m add cascade -label "Choose query" -menu .top.options.m.clist
.top.options.m add command -label "Define query" -command {new-query-dialog}
.top.options.m add cascade -label "Edit query" -menu .top.options.m.slist
menu .top.options.m.clist
menu .top.options.m.slist
cascade-query-list

menubutton .top.help -text "Help" -menu .top.help.m
menu .top.help.m

.top.help.m add command -label "Help on help" \
        -command {tkerror "Help on help not available. Sorry"}
.top.help.m add command -label "About" \
        -command {tkerror "About not available. Sorry"}

pack .top.file .top.target .top.service .top.rset .top.options -side left
pack .top.help -side right

index-lines .lines 1 $queryButtonsFind [lindex $queryInfo 0] activate-index

button .mid.search -width 7 -text {Search} -command search-request \
        -state disabled
button .mid.scan -width 7 -text {Scan} \
        -command [list scan-request "@attr 1=4 @attr 5=1 @attr 4=1"] -state disabled 
button .mid.present -width 7 -text {Present} -command [list present-more 10] \
        -state disabled

button .mid.clear -width 7 -text {Clear} -command index-clear
pack .mid.search .mid.scan .mid.present .mid.clear -side left \
        -fill y -padx 5 -pady 3

listbox .data.list -yscrollcommand {.data.scroll set} -font fixed -geometry 20x2
scrollbar .data.scroll -orient vertical -border 1
pack .data.list -side left -fill both -expand yes
pack .data.scroll -side right -fill y
.data.scroll config -command {.data.list yview}

button .bot.logo  -bitmap @book1 -command cancel-operation
frame .bot.a
pack .bot.a -side left -fill x
pack .bot.logo -side right -padx 2 -pady 2

message .bot.a.target -text "" -aspect 1000 -border 1

label .bot.a.status -text "Not connected" -width 15 -relief \
        sunken -anchor w -border 1
label .bot.a.set -text "" -width 5 -relief \
        sunken -anchor w -border 1
label .bot.a.message -text "" -width 15 -relief \
        sunken -anchor w -border 1

pack .bot.a.target -side top -anchor nw -padx 2 -pady 2
pack .bot.a.status .bot.a.set .bot.a.message \
        -side left -padx 2 -pady 2

ir z39

show-logo 1


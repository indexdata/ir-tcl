#
# $Log: client.tcl,v $
# Revision 1.21  1995-05-11 15:34:46  adam
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

set queryTypes {Simple}
set queryButtons { { {I 0} {I 1} {I 2} } }
set queryInfo { { {Title ti} {Author au} {Subject sh} {Any any} } }

wm minsize . 300 250

if {[file readable "~/.tk-c"]} {
    source "~/.tk-c"
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
    pack $w.bot.left -side left -expand yes -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command ${ok-action}
    pack $w.bot.left.ok -expand yes -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command "destroy $w"
    pack $w.bot.cancel -side left -expand yes    

    if {$g} {
        # Grab ...
        grab $w
        tkwait window $w
    }
}

proc top-down-ok-cancelx {w buttonList g} {
    set i 0
    set l [llength $buttonList]

    frame $w.bot.$i -relief sunken -border 1
    pack $w.bot.$i -side left -expand yes -padx 5 -pady 5
    button $w.bot.$i.ok -text [lindex $buttonList $i] \
            -command [lindex $buttonList [expr $i+1]]
    pack $w.bot.$i.ok -expand yes -padx 3 -pady 3 -side left

    incr i 2
    while {$i < $l} {
        button $w.bot.$i -text [lindex $buttonList $i] \
                -command [lindex $buttonList [expr $i+1]]
        pack $w.bot.$i -expand yes -padx 3 -pady 3 -side left
        incr i 2
    }
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command "destroy $w"
    pack $w.bot.cancel -side left -expand yes    
    
    if {$g} {
        # Grab ...
        grab $w
        tkwait window $w
    }
}

proc show-target {target} {
    .bot.target configure -text "$target"
}

proc show-busy {v1 v2} {
    global busy
    if {$busy != 0} {
        .bot.status configure -fg $v1
        after 200 [list show-busy $v2 $v1]
    }
}
        
proc show-status {status b} {
    global busy
    global statusbg
    .bot.status configure -text "$status"
    .bot.status configure -fg black
    if {$b != 0} {
        if {$busy == 0} {
            set busy $b   
            show-busy red blue
        }
        #        . config -cursor {watch black white}
    } else {
        #        . config -cursor {top_left_arrow black white}
        puts "Normal"
    }
    set busy $b
}

proc show-message {msg} {
    .bot.message configure -text "$msg"
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

proc show-full-marc {no} {
    global setNo

    set w .full-marc

    if {[winfo exists $w]} {
        $w.top.record delete 0.0 end
        set new 0
    } else {

        toplevel $w

        wm minsize $w 200 200
        
        frame $w.top -relief raised -border 1
        frame $w.bot -relief raised -border 1

        pack  $w.top -side top -fill both -expand yes
        pack  $w.bot -fill both

        text $w.top.record -width 60 -height 12 -wrap word \
                -yscrollcommand [list $w.top.s set]
        scrollbar $w.top.s -command [list $w.top.record yview]

        set new 1
    }
    incr no
    
    set r [z39.$setNo recordMarc $no line * * *]

    $w.top.record tag configure marc-tag -foreground blue
    $w.top.record tag configure marc-data -foreground black
    $w.top.record tag configure marc-id -foreground red

    foreach line $r {
        set tag [lindex $line 0]
        set indicator [lindex $line 1]
        set fields [lindex $line 2]

        if {$indicator != ""} {
            insertWithTags $w.top.record "$tag $indicator" marc-tag
        } else {
            insertWithTags $w.top.record "$tag    " marc-tag
        }
        foreach field $fields {
            set id [lindex $field 0]
            set data [lindex $field 1]
            if {$id != ""} {
                insertWithTags $w.top.record " $id " marc-id
            }
            set start [$w.top.record index insert]
            insertWithTags $w.top.record $data {}
        }
        $w.top.record insert end "\n"
    }
    if {$new} {
        bind $w <Return> {destroy .full-marc}
        
        pack $w.top.s -side right -fill y
        pack $w.top.record -expand yes -fill both
        
        frame $w.bot.left -relief sunken -border 1
        pack $w.bot.left -side left -expand yes -padx 5 -pady 5
        button $w.bot.left.close -width 6 -text {Close} \
                -command {destroy .full-marc}
        pack $w.bot.left.close -expand yes -padx 3 -pady 3
        button $w.bot.edit -width 6 -text {Edit} \
                -command {destroy .full-marc}
        pack $w.bot.edit -side left -expand yes
    }
}

proc update-target-hotlist {target} {
    global hotTargets

    set len [llength $hotTargets]
    if {$len > 0} {
        .top.target.m delete 5 [expr 5+[llength $hotTargets]]
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
                "reopen-target $target {}"
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
    set profile($target) $profile(Default)
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
    # z39 idAuthentication [lindex $profile($target) 3]
    z39 maximumRecordSize [lindex $profile($target) 4]
    z39 preferredMessageSize [lindex $profile($target) 5]
    puts -nonewline "maximumRecordSize="
    puts [z39 maximumRecordSize]
    puts -nonewline "preferredMessageSize="
    puts [z39 preferredMessageSize]
    if {$base == ""} {
        z39 databaseNames [lindex [lindex $profile($target) 7] 0]
    } else {
        z39 databaseNames $base
    }
    z39 failback [list fail-response $target]
    z39 callback [list connect-response $target]
    z39 connect [lindex $profile($target) 1]:[lindex $profile($target) 2]
    show-status {Connecting} 1
    set hostid $target
    .top.target.m disable 0
    .top.target.m enable 1
}

proc close-target {} {
    global hostid

    set hostid Default
    z39 disconnect
    show-target {None}
    show-status {Not connected} 0
    show-message {}
    .top.target.m disable 1
    .top.target.m enable 0
    .top.search configure -state disabled
    .mid.search configure -state disabled
    .mid.scan configure -state disabled
}

proc load-set-action {} {
    global setNo

    incr setNo
    ir-set z39.$setNo

    set fname [.load-set.top.filename.entry get]
    destroy .load-set
    if {$fname != ""} {
        init-title-lines

        show-status {Loading} 1
        z39.$setNo loadFile $fname

        set no [z39.$setNo numberOfRecordsReturned]
        add-title-lines $setNo $no 1
    }
    show-status {Ready} 0
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
    
    z39 callback {init-response}
    z39 init
    show-status {Initializing} 1
}

proc init-response {} {
    show-status {Ready} 0
    .top.search configure -state normal
    .mid.search configure -state normal
    .mid.scan configure -state normal
}

proc search-request {} {
    global setNo
    global profile
    global hostid

    set target $hostid

    set query [index-query]
    if {$query==""} {
        return
    }
    incr setNo
    ir-set z39.$setNo


    if {[lindex $profile($target) 10] != ""} {
        z39.$setNo setName $setNo
    } else {
        z39.$setNo setName Default
    }
    if {[lindex $profile($target) 8] != ""} {
        z39 query rpn
    }
    if {[lindex $profile($target) 9] != ""} {
        z39 query ccl
    }
    z39 callback {search-response}
    z39.$setNo search $query
    show-status {Search} 1
}

proc scan-request {} {
    set w .scan-window

    global profile
    global hostid

    set target $hostid

    ir-scan z39.scan

    z39 callback {scan-response}
    if {![winfo exists $w]} {
        toplevel $w
        
        wm title $w "Scan"
        
        wm minsize $w 200 200

        top-down-window $w
        
        listbox $w.top.list -yscrollcommand [list $w.top.scroll set] \
                -font fixed -geometry 50x14
        scrollbar $w.top.scroll -orient vertical -border 1
        pack $w.top.list -side left -fill both -expand yes
        pack $w.top.scroll -side right -fill y
        $w.top.scroll config -command [list $w.top.list yview]

        top-down-ok-cancelx $w [list {Close} [list destroy $w]] 0 
    }
    z39.scan numberOfTermsRequested 100
    z39.scan scan ti=0
    
    show-status {Scan} 1
}

proc scan-response {} {
    set w .scan-window
    set m [z39.scan numberOfEntriesReturned]
    puts $m
    for {set i 0} {$i < $m} {incr i} {
        set term [lindex [z39.scan scanLine $i] 1]
        set nostr [format "%7d" [lindex [z39.scan scanLine $i] 2]]

        $w.top.list insert end "$nostr $term"
    }
    show-status {Ready} 0
}

proc search-response {} {
    global setNo
    global setOffset
    global setMax

    init-title-lines
    show-status {Ready} 0
    show-message "[z39.$setNo resultCount] hits"
    set setMax [z39.$setNo resultCount]
    puts $setMax
    if {$setMax == 0} {
        set status [z39.$setNo responseStatus]
        if {[lindex $status 0] == "NSD"} {
            set code [lindex $status 1]
            set msg [lindex $status 2]
            set addinfo [lindex $status 3]
            tkerror "NSD$code: $msg: $addinfo"
        }
        return
    }
    if {$setMax > 4} {
        set setMax 4
    }
    z39 callback {present-response}
    set setOffset 1
    z39.$setNo present $setOffset $setMax
    show-status {Retrieve} 1
}

proc present-more {number} {
    global setNo
    global setOffset
    global setMax

    puts "present-more"
    if {$setNo == 0} {
	return
    }
    set max [z39.$setNo resultCount]
    if {$max <= $setMax} {
        return
    }
    puts "max=$max"
    puts "setOffset=$setOffset"
    if {$number == ""} {
        set setMax $max
    } else {
        incr setMax $number
    }
    z39 callback {present-response}
    z39.$setNo present $setOffset [expr $setMax - $setOffset + 1]
    show-status {Retrieve} 1
}

proc init-title-lines {} {
    .data.list delete 0 end
}

proc add-title-lines {setno no offset} {
    for {set i 0} {$i < $no} {incr i} {
        set o [expr $i + $offset]
        set title [lindex [z39.$setno recordMarc $o field 245 * a] 0]
        set year  [lindex [z39.$setno recordMarc $o field 260 * c] 0]
        set nostr [format "%5d" $o]
        .data.list insert end "$nostr $title - $year"
    }
}

proc present-response {} {
    global setNo
    global setOffset
    global setMax

    puts "In present-response"
    set no [z39.$setNo numberOfRecordsReturned]
    puts "Returned $no records, setOffset $setOffset"
    add-title-lines $setNo $no $setOffset
    set setOffset [expr $setOffset + $no]
    set status [z39.$setNo responseStatus]
    if {[lindex $status 0] == "NSD"} {
        show-status {Ready} 0
        set code [lindex $status 1]
        set msg [lindex $status 2]
        set addinfo [lindex $status 3]
        tkerror "NSD$code: $msg: $addinfo"
        return
    }
    if {$no > 0 && $setOffset <= $setMax} {
        z39.$setNo present $setOffset [expr $setMax - $setOffset + 1]
    } else {
        show-status {Finished} 0
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

    set w .setup-${target}.top

    #set w .protocol-setup.top
    
    set b {}
    set settingsChanged 1
    set len [$w.databases.list size]
    for {set i 0} {$i < $len} {incr i} {
        lappend b [$w.databases.list get $i]
    }
    set profile($target) [list [$w.description.entry get] \
            [$w.host.entry get] \
            [$w.port.entry get] \
            [$w.idAuthentication.entry get] \
            [$w.maximumRecordSize.entry get] \
            [$w.preferredMessageSize.entry get] \
            $csRadioType \
            $b \
            $RPNCheck \
            $CCLCheck \
            $ResultSetCheck \
            $protocolRadioType ]

    cascade-target-list
    puts $profile($target)
    destroy .setup-${target}
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
    set w .setup-${target}
    
    ${w}.top.databases.list insert end \
            [.database-select.top.database.entry get]
    destroy .database-select
}

proc add-database {target} {
    set w .database-select

    set oldFocus [focus]
    toplevel $w

    place-force $w .setup-${target}

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
    set w .setup-${target}
    
    foreach i [lsort -decreasing \
            [$w.top.databases.list curselection]] {
        $w.top.databases.list delete $i
    }
}

proc protocol-setup {target} {
    set w .setup-$target

    global profile
    global csRadioType
    global protocolRadioType
    global RPNCheck
    global CCLCheck
    global ResultSetCheck

    toplevel $w

    wm title $w "Setup $target"
    place-force $w .

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
            [list protocol-setup-action $target] [list destroy $w]
    
    foreach sub {description host port idAuthentication \
            maximumRecordSize preferredMessageSize} {
        puts $sub
        bind $w.top.$sub.entry <Control-a> "add-database $target"
        bind $w.top.$sub.entry <Control-d> "delete-database $target"
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
            -command "add-database $target"
    button $w.top.databases.delete -text "Delete" \
            -command "delete-database $target"
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
    top-down-ok-cancel $w [list protocol-setup-action $target] 0
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
                            -command "reopen-target $n $b"
                }
            } else {
                .top.target.m.clist add command -label $n \
                        -command "reopen-target $n {}"
            }
        }
    }
    .top.target.m.slist delete 0 last
    foreach n [array names profile] {
        if {$n != "Default"} {
            .top.target.m.slist add command -label $n \
                    -command "protocol-setup $n"
        }
    }
}

proc cascade-query-list {} {
    global queryTypes

    set i 0
    .top.query.m.slist delete 0 last
    foreach n $queryTypes {
        .top.query.m.slist add command -label $n \
                -command [list query-setup $i]
        incr i
    }

    set i 0
    .top.query.m.clist delete 0 last
    foreach n $queryTypes {
        .top.query.m.clist add command -label $n \
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

    set f [open "~/.tk-c" w]
    puts $f "# Setup file"
    puts $f "set hotTargets \{ $hotTargets \}"

    foreach n [array names profile] {
        puts -nonewline $f "set profile($n) \{"
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
    destroy .
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
    top-down-ok-cancelx $w [list \
            {Ok} [list query-setup-action $queryNo] \
            {Add index} [list query-add-index $queryNo] \
            {Add line} [list query-add-line $queryNo] \
            {Delete line} [list query-del-line $queryNo]] 0
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
            if {$qs != ""} {
                set qs "${qs} and "
            }
            if {$attr != ""} {
                set qs "${qs}${attr}="
            }
            set qs "${qs}(${term})"
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
.top.file.m add command -label "Load Set" -command {load-set}
.top.file.m add separator
.top.file.m add command -label "Exit" -command {exit-action}

menubutton .top.target -text "Target" -underline 0 -menu .top.target.m
menu .top.target.m
.top.target.m add cascade -label "Connect" -menu .top.target.m.clist
.top.target.m add command -label "Disconnect" -command {close-target}
#.top.target.m add command -label "Initialize" -command {init-request}
.top.target.m add cascade -label "Setup" -menu .top.target.m.slist
.top.target.m add command -label "Setup new" -command {define-target-dialog}
.top.target.m add separator
set-target-hotlist

.top.target.m disable 1

menu .top.target.m.clist
menu .top.target.m.slist
cascade-target-list

menubutton .top.search -text "Search" -underline 0 -menu .top.search.m
menu .top.search.m
.top.search.m add command -label "Database" -command {database-select}
.top.search.m add cascade -label "Query type" -menu .top.search.m.querytype
menu .top.search.m.querytype
.top.search.m.querytype add radiobutton -label "RPN"
.top.search.m.querytype add radiobutton -label "CCL"
.top.search.m add cascade -label "Present" -menu .top.search.m.present
menu .top.search.m.present
.top.search.m.present add command -label "More" -command [list present-more 10]
.top.search.m.present add command -label "All" -command [list present-more {}]
.top.search configure -state disabled

menubutton .top.query -text "Query" -underline 0 -menu .top.query.m
menu .top.query.m
.top.query.m add cascade -label "Choose" -menu .top.query.m.clist
.top.query.m add command -label "Define" -command {new-query-dialog}
.top.query.m add cascade -label "Edit" -menu .top.query.m.slist
menu .top.query.m.clist
menu .top.query.m.slist
cascade-query-list

menubutton .top.help -text "Help" -menu .top.help.m
menu .top.help.m

.top.help.m add command -label "Help on help" -command {puts "Help on help"}
.top.help.m add command -label "About" -command {puts "About"}

pack .top.file .top.target .top.query .top.search -side left
pack .top.help -side right

index-lines .lines 1 $queryButtonsFind [lindex $queryInfo 0] activate-index

button .mid.search -width 6 -text {Search} -command search-request \
        -state disabled
button .mid.scan -width 6 -text {Scan} -command scan-request \
        -state disabled
button .mid.clear -width 6 -text {Clear} -command index-clear
pack .mid.search .mid.scan .mid.clear -side left -padx 5 -pady 3

listbox .data.list -yscrollcommand {.data.scroll set} -font fixed
scrollbar .data.scroll -orient vertical -border 1
pack .data.list -side left -fill both -expand yes
pack .data.scroll -side right -fill y
.data.scroll config -command {.data.list yview}

message .bot.target -text "None" -aspect 1000 -relief sunken -border 1
label .bot.status -text "Not connected" -width 12 -relief \
        sunken -anchor w -border 1
label .bot.set -textvariable setNo -width 5 -relief \
        sunken -anchor w -border 1
label .bot.message -text "" -width 14 -relief \
        sunken -anchor w -border 1
pack .bot.target .bot.status .bot.set .bot.message -anchor nw \
        -side left -padx 2 -pady 2

bind .data.list <Double-Button-1> {set indx [.data.list nearest %y]
show-full-marc $indx}

ir z39

#
# $Log: client.tcl,v $
# Revision 1.10  1995-03-20 15:24:06  adam
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

set profile(Default) {{} {} {210} {} 16384 8192 tcpip {}}
set hostid Default
set settingsChanged 0
set setNo 0

wm minsize . 360 200

if {[file readable "~/.tk-c"]} {
    source "~/.tk-c"
}

proc top-down-window {w} {
    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    
    pack  $w.top $w.bot -side top -fill both -expand yes
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

        #        pack  $w.top $w.bot -side top -fill both -expand yes
        pack  $w.top -side top -fill both -expand yes
        pack  $w.bot -fill both

        text $w.top.record -width 60 -height 10 -wrap word \
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
                "menu-open-target $target {}"
        incr i
        if {$i > 8} {
             break
        }
    }
}

proc menu-open-target {target base} {
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

proc open-target {target base} {
    global profile

    .top.target.m disable 0
    .top.target.m enable 1
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
    show-target $target
    z39 connect  [lindex $profile($target) 1]:[lindex $profile($target) 2]
    init-request
}

proc load-set-action {} {
    global setNo

    incr setNo
    ir-set z39.$setNo

    set fname [.load-set.top.filename.entry get]
    destroy .load-set
    if {$fname != ""} {
        .data.list delete 0 end

        show-status {Loading} 1
        z39.$setNo loadFile $fname

        set no [z39.$setNo numberOfRecordsReturned]
        add-title-lines $no 1
    }
    show-status {Ready} 0
}

proc load-set {} {
    set w .load-set

    toplevel $w

    place-force $w .

    top-down-window $w

    frame $w.top.filename
    
    pack $w.top.filename -side top -anchor e -pady 2
    
    entry-fields $w.top {filename} \
            {{Filename:}} \
            {load-set-action} {destroy .load-set}
    
    top-down-ok-cancel $w {load-set-action} 1
}

proc init-request {} {
    global setNo
    
    z39 callback {init-response}
    z39 init
    show-status {Initializing} 1
    set setNo 0
}

proc init-response {} {
    show-status {Ready} 0
    pack .mid.searchlabel .mid.searchentry -side left
    bind .mid.searchentry <Return> search-request
    focus .mid.searchentry
}

proc search-request {} {
    global setNo

    incr setNo
    ir-set z39.$setNo

    z39 callback {search-response}
    z39.$setNo search [.mid.searchentry get]
    show-status {Search} 1
}

proc search-response {} {
    global setNo
    global setOffset
    global setMax

    .data.list delete 0 end
    show-status {Ready} 0
    show-message "[z39.$setNo resultCount] hits"
    set setMax [z39.$setNo resultCount]
    puts $setMax
    if {$setMax == 0} {
        return
    }
    if {$setMax > 10} {
        set setMax 10
    }
    z39 callback {present-response}
    set setOffset 1
    z39.$setNo present 1 $setMax
    show-status {Retrieve} 1
}

proc add-title-lines {no offset} {
    global setNo

    for {set i 0} {$i < $no} {incr i} {
        set o [expr $i + $offset]
        set title [lindex [z39.$setNo recordMarc $o field 245 * a] 0]
        set year  [lindex [z39.$setNo recordMarc $o field 260 * c] 0]
        set nostr [format "%3d" $o]
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
    add-title-lines $no $setOffset
    set setOffset [expr $setOffset + $no]
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

proc close-target {} {
    pack forget .mid.searchlabel .mid.searchentry
    z39 disconnect
    show-target {None}
    show-status {Not connected} 0
    show-message {}
    .top.target.m disable 1
    .top.target.m enable 0
}

proc protocol-setup-action {target} {
    global profile
    global csRadioType
    global settingsChanged

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
            $b]

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
    frame $w.top.query -relief ridge -border 2
    frame $w.top.databases -relief ridge -border 2

    # Maximum/preferred/idAuth ...
    pack $w.top.description $w.top.host $w.top.port \
            $w.top.idAuthentication $w.top.maximumRecordSize \
            $w.top.preferredMessageSize -side top -anchor e -pady 2
    #-anchor e
    
    entry-fields $w.top {description host port idAuthentication \
            maximumRecordSize preferredMessageSize} \
            {{Description:} {Host:} {Port:} {Id Authentification:} \
            {Maximum Record Size:} {Preferred Message Size:}} \
            [list protocol-setup-action $target] [list destroy $w]
    
    $w.top.description.entry insert 0 [lindex $profile($target) 0]
    $w.top.host.entry insert 0 [lindex $profile($target) 1]
    $w.top.port.entry insert 0 [lindex $profile($target) 2]
    $w.top.idAuthentication.entry insert 0 [lindex $profile($target) 3]
    $w.top.maximumRecordSize.entry insert 0 [lindex $profile($target) 4]
    $w.top.preferredMessageSize.entry insert 0 [lindex $profile($target) 5]

    # Databases ....
    pack $w.top.databases -side left -pady 6 -padx 6 -expand yes -fill x

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
    set csRadioType [lindex $profile($target) 6]

    pack $w.top.cs-type -pady 6 -padx 6 -side top
    
    label $w.top.cs-type.label -text "Transport" 
    radiobutton $w.top.cs-type.tcpip -text "TCP/IP" \
            -command {puts tcp/ip} -variable csRadioType -value tcpip
    radiobutton $w.top.cs-type.mosi -text "MOSI" \
            -command {puts mosi} -variable csRadioType -value mosi
    
    pack $w.top.cs-type.label $w.top.cs-type.tcpip $w.top.cs-type.mosi \
            -padx 4 -side top -fill x

    # Query ...
    pack $w.top.query -pady 6 -padx 6 -side top

    label $w.top.query.label -text "Query support" -anchor e
    checkbutton $w.top.query.c1 -text "CCL query"   
    checkbutton $w.top.query.c2 -text "RPN query"
    checkbutton $w.top.query.c3 -text "Result sets"

    pack $w.top.query.label -side top 
    pack $w.top.query.c1 $w.top.query.c2 $w.top.query.c3 \
            -padx 4 -side top -fill x

    foreach sub [winfo children $w.top] {
        puts $sub
        bind $sub <Control-a> "add-database $target"
    }
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

    if {$hostid == ""} {
        set hostid Default
    }

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
                            -command "menu-open-target $n $b"
                }
            } else {
                .top.target.m.clist add command -label $n \
                        -command "menu-open-target $n {}"
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

proc save-settings {} {
    global hotTargets 
    global profile
    global settingsChanged

    set f [open "~/.tk-c" w]
    puts $f "# Setup file"
    puts $f "set hotTargets \{ $hotTargets \}"

    foreach n [array names profile] {
        puts -nonewline $f "set profile($n) \{"
        puts -nonewline $f $profile($n)
        puts $f "\}"
    }
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

frame .top  -border 1 -relief raised
frame .mid  -border 1 -relief raised
frame .data -border 1 -relief raised
frame .bot  -border 1 -relief raised
pack .top .mid -side top -fill x
pack .data      -side top -fill both -expand yes
pack .bot      -fill x

menubutton .top.file -text "File" -menu .top.file.m
menu .top.file.m
.top.file.m add command -label "Save settings" -command {save-settings}
.top.file.m add command -label "Load Set" -command {load-set}
.top.file.m add separator
.top.file.m add command -label "Exit" -command {exit-action}

menubutton .top.target -text "Target" -menu .top.target.m
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

menubutton .top.database -text "Database" -menu .top.database.m
menu .top.database.m
.top.database.m add command -label "Select ..." -command {database-select}
.top.database.m add command -label "Add ..." -command {puts "Add"}

menubutton .top.help -text "Help" -menu .top.help.m
menu .top.help.m

.top.help.m add command -label "Help on help" -command {puts "Help on help"}
.top.help.m add command -label "About" -command {puts "About"}

pack .top.file .top.target .top.database -side left
pack .top.help -side right

label .mid.searchlabel -text {Search:}
entry .mid.searchentry -width 40 -relief sunken

bind .mid.searchentry <Left> {left-cursor .mid.searchentry}
bind .mid.searchentry <Right> {right-cursor .mid.searchentry}

listbox .data.list -yscrollcommand {.data.scroll set}
scrollbar .data.scroll -orient vertical -border 1
pack .data.list -side left -fill both -expand yes
pack .data.scroll -side right -fill y
.data.scroll config -command {.data.list yview}

message .bot.target -text "None" -aspect 1000 -relief sunken -border 1
label .bot.status -text "Not connected" -width 12 -relief \
    sunken -anchor w -border 1
label .bot.message -text "" -width 20 -relief \
    sunken -anchor w -border 1
pack .bot.target .bot.status .bot.message -anchor nw -side left -padx 2 -pady 2

bind .data.list <Double-Button-1> {set indx [.data.list nearest %y]
show-full-marc $indx}

ir z39

#
# $Log: client.tcl,v $
# Revision 1.4  1995-03-14 17:32:29  adam
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

wm minsize . 360 200

if {[file readable "~/.tk-c"]} {
    source "~/.tk-c"
}

proc top-down-window {w} {
    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    
    pack  $w.top $w.bot -side top -fill both -expand yes
}

proc top-down-ok-cancel {w ok-action} {
    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command ${ok-action}
    pack $w.bot.left.ok -expand yes -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command "destroy $w"
    pack $w.bot.cancel -side left -expand yes    
    
    # Grab ...
    grab $w
    
    tkwait window $w
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

        text $w.top.record -width 60 -height 10 \
                -yscrollcommand "$w.top.s set"
        scrollbar $w.top.s -command "$w.top.record yview"

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
        .top.target.m add command -label $target -command \
            "menu-open-target $target"
        incr i
        if {$i > 8} {
             break
        }
    }
}

proc menu-open-target {target} {
    open-target $target
    update-target-hotlist $target
}

proc open-target-action {} {
    set host [.target-connect.top.host.entry get]
    set port [.target-connect.top.port.entry get]

    if {$host == ""} {
        return
    }
    if {$port == ""} {
        set port 210
    }
    open-target "${host}:${port}"
    update-target-hotlist ${host}:${port}
    destroy .target-connect
}

proc open-target {target} {
    z39 disconnect
    global csRadioType
    z39 comstack ${csRadioType}
    show-target $target
    z39 connect $target

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
    
    top-down-ok-cancel $w {load-set-action}
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
    if {$setMax > 16} {
        set setMax 16
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
        .data.list insert end "$title - $year"
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
    if { $setOffset <= $setMax} {
        z39.$setNo present $setOffset [expr $setMax - $setOffset + 1]
    } else {
        show-status {Finished} 0
    }
}

proc bind-fields {list returnAction escapeAction} {
    set max [expr [llength $list]-1]
    for {set i 0} {$i < $max} {incr i} {
        bind [lindex $list $i] <Return> $returnAction
        bind [lindex $list $i] <Escape> $escapeAction
        bind [lindex $list $i] <Tab> [list focus [lindex $list [expr $i+1]]]
    }
    bind [lindex $list $i] <Return> $returnAction
    bind [lindex $list $i] <Escape> $escapeAction
    bind [lindex $list $i] <Tab>    [list focus [lindex $list 0]]
    focus [lindex $list 0]
}

proc entry-fields {parent list tlist returnAction escapeAction} {
    set alist {}
    set i 0
    foreach field $list {
        set label ${parent}.${field}.label
        set entry ${parent}.${field}.entry
        label $label -text [lindex $tlist $i] -anchor e
        entry $entry -width 30 -relief sunken
        pack $label -side left
        pack $entry -side right
        lappend alist $entry
        incr i
    }
    bind-fields $alist $returnAction $escapeAction
}

proc open-target-dialog {} {
    set w .target-connect

    toplevel $w

    place-force $w .

    top-down-window $w

    frame $w.top.host
    frame $w.top.port

    pack $w.top.host $w.top.port \
            -side top -anchor e -pady 2 

    entry-fields $w.top {host port } \
            {{Hostname:} {Port number:}} \
            {open-target-action} {destroy .target-connect}

    top-down-ok-cancel $w {open-target-action}
}

proc close-target {} {
    pack forget .mid.searchlabel .mid.searchentry
    z39 disconnect
    show-target {None}
    show-status {Not connected} 0
    show-message {}
}

proc protocol-setup-action {} {
    destroy .protocol-setup
}


proc place-force {window parent} {
    set g [wm geometry $parent]

    set p1 [string first + $g]
    set p2 [string last + $g]

    set x [expr 40+[string range $g [expr $p1 +1] [expr $p2 -1]]]
    set y [expr 60+[string range $g [expr $p2 +1] end]]
    wm geometry $window +${x}+${y}
}

proc protocol-setup {} {
    set w .protocol-setup

    toplevel $w

    place-force $w .

    top-down-window $w
    
    frame $w.top.description
    frame $w.top.idAuthentification
    frame $w.top.maximumMessageSize
    frame $w.top.preferredMessageSize
    frame $w.top.cs-type -relief ridge -border 2
    frame $w.top.query -relief ridge -border 2
    
    # Maximum/preferred/idAuth ...
    pack $w.top.description \
            $w.top.idAuthentification $w.top.maximumMessageSize \
            $w.top.preferredMessageSize -side top -anchor e -pady 2 
    
    entry-fields $w.top {description idAuthentification maximumMessageSize \
            preferredMessageSize} \
            {{Description:} {Id Authentification:} {Maximum Message Size:}
    {Preferred Message Size:}} \
            {protocol-setup-action} {destroy .protocol-setup}
    
    # Transport ...
    pack $w.top.cs-type -side left -pady 2 -padx 2
    
    global csRadioType
    
    label $w.top.cs-type.label -text "Transport" -anchor e
    radiobutton $w.top.cs-type.tcpip -text "TCP/IP" \
            -command {puts tcp/ip} -variable csRadioType -value tcpip
    radiobutton $w.top.cs-type.mosi -text "MOSI" \
            -command {puts mosi} -variable csRadioType -value mosi
    
    pack $w.top.cs-type.label $w.top.cs-type.tcpip $w.top.cs-type.mosi \
            -padx 4 -side top -fill x
    
    # Query ...
    pack $w.top.query -side right -pady 2 -padx 2 -expand yes

    label $w.top.query.label -text "Query support" -anchor e
    checkbutton $w.top.query.c1 -text "CCL query"   
    checkbutton $w.top.query.c2 -text "RPN query"
    checkbutton $w.top.query.c3 -text "Result sets"

    pack $w.top.query.label -side top -anchor w
    pack $w.top.query.c1 $w.top.query.c2 $w.top.query.c3 \
            -padx 4 -side left -fill x
    
    top-down-ok-cancel $w {protocol-setup-action}
}

proc database-select-action {} {
    z39 databaseNames [.database-select.top.database.entry get]
    destroy .database-select
}

proc database-select {} {
    set w .database-select

    toplevel $w

    place-force $w .

    top-down-window $w

    frame $w.top.database

    pack $w.top.database -side top -anchor e -pady 2
    
    entry-fields $w.top {database} \
            {{Database:}} \
            {database-select-action} {destroy .database-select}

    top-down-ok-cancel $w {database-select-action}
}

proc save-settings {} {
    global hotTargets 

    set f [open "~/.tk-c" w]
    puts $f "# Setup file"
    puts $f "set hotTargets \{ $hotTargets \}"
    close $f
}

frame .top -border 1 -relief raised
frame .mid  -border 1 -relief raised
frame .data -border 1 -relief raised
frame .bot -border 1 -relief raised
pack .top .mid -side top -fill x
pack .data      -side top -fill both -expand yes
pack .bot      -fill x

menubutton .top.file -text "File" -menu .top.file.m
menu .top.file.m
.top.file.m add command -label "Save settings" -command {save-settings}
.top.file.m add command -label "Load Set" -command {load-set}
.top.file.m add separator
.top.file.m add command -label "Exit" -command {destroy .}

menubutton .top.target -text "Target" -menu .top.target.m
menu .top.target.m
.top.target.m add command -label "Connect" -command {open-target-dialog}
.top.target.m add command -label "Disconnect" -command {close-target}
.top.target.m add command -label "Initialize" -command {init-request}
.top.target.m add command -label "Setup" -command {protocol-setup}
.top.target.m add separator
set-target-hotlist

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
entry .mid.searchentry -width 50 -relief sunken

listbox .data.list -yscrollcommand {.data.scroll set}
#-geometry 50x10
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

set setNo 0
ir z39
z39 comstack tcpip
set csRadioType [z39 comstack]

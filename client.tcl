#
# $Log: client.tcl,v $
# Revision 1.2  1995-03-10 18:00:15  adam
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
wm maxsize . 800 800

if {[file readable "~/.tk-c"]} {
    source "~/.tk-c"
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

proc init-request {} {
    global SetNo

    z39 callback {init-response}
    z39 init
    show-status {Initializing} 1
    set SetNo 0
}

proc init-response {} {
    show-status {Ready} 0
    pack .mid.searchlabel .mid.searchentry -side left
    bind .mid.searchentry <Return> search-request
    focus .mid.searchentry
}

proc search-request {} {
    global SetNo

    incr SetNo
    ir-set z39.$SetNo

    z39 callback {search-response}
    z39.$SetNo search [.mid.searchentry get]
    show-status {Search} 1
}

proc search-response {} {
    global SetNo
    global setOffset
    global setMax

    .data.list delete 0 end
    show-status {Ready} 0
    show-message "[z39.$SetNo resultCount] hits"
    set setMax [z39.$SetNo resultCount]
    puts $setMax
    if {$setMax > 16} {
        set setMax 16
    }
    z39 callback {present-response}
    set setOffset 1
    z39.$SetNo present 1 $setMax
    show-status {Retrieve} 1
}

proc present-response {} {
    global SetNo
    global setOffset
    global setMax

    puts "In present-response"
    set no [z39.$SetNo numberOfRecordsReturned]
    puts "Returned $no records, setOffset $setOffset"
    for {set i 0} {$i < $no} {incr i} {
        set o [expr $i + $setOffset]
        set title [lindex [z39.$SetNo getRecord $o 245 a] 0]
        set year  [lindex [z39.$SetNo getRecord $o 260 c] 0]
        .data.list insert end "$title - $year"
    }
    set setOffset [expr $setOffset + $no]
    if { $setOffset <= $setMax} {
        z39.$SetNo present $setOffset [expr $setMax - $setOffset + 1]
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
    
    frame $w.top -relief sunken -border 1
    frame $w.bot -relief sunken -border 1
    
    pack  $w.top $w.bot -side top -fill both -expand yes

    frame $w.top.host
    frame $w.top.port

    pack $w.top.host $w.top.port \
            -side top -anchor e -pady 2 

    entry-fields $w.top {host port } \
            {{Hostname:} {Port number:}} \
            {open-target-action} {destroy .target-connect}

    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command {open-target-action}
    pack $w.bot.left.ok -expand yes -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command {destroy .target-connect}
    pack $w.bot.cancel -side left -expand yes

    grab $w

    tkwait window $w
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

    frame $w.top -relief sunken -border 1
    frame $w.bot -relief sunken -border 1
    
    pack  $w.top $w.bot -side top -fill both -expand yes

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

# Buttons ...
    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command {protocol-setup-action}
    pack $w.bot.left.ok -expand yes -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command "destroy $w"
    pack $w.bot.cancel -side left -expand yes    

# Grab ...
    grab $w

    tkwait window $w

}

proc database-select-action {} {
    z39 databaseNames [.database-select.top.database.entry get]
    destroy .database-select
}

proc database-select {} {
    set w .database-select

    toplevel $w

    place-force $w .

    frame $w.top -relief sunken -border 1
    frame $w.bot -relief sunken -border 1

    pack  $w.top $w.bot -side top -fill both -expand yes

    frame $w.top.database

# Database select
    pack $w.top.database -side top -anchor e -pady 2

    entry-fields $w.top {database} \
            {{Database:}} \
            {database-select-action} {destroy .database-select}

# Buttons ...
    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -padx 5 -pady 5
    button $w.bot.left.ok -width 6 -text {Ok} \
            -command {protocol-setup-action}
    pack $w.bot.left.ok -expand yes -padx 3 -pady 3
    button $w.bot.cancel -width 6 -text {Cancel} \
            -command "destroy .database-select"
    pack $w.bot.cancel -side left -expand yes    

# Grab ...
    grab $w

    tkwait window $w
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

for {set i 0} {$i < 30} {incr i} {
    .data.list insert end "Record $i"
}

bind .data.list <Double-Button-1> {set indx [.data.list nearest %y]
puts "y=%y index $indx" }

ir z39
z39 comstack tcpip
set csRadioType [z39 comstack]

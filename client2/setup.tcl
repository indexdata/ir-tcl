proc print-date {w msg date} {
    frame $w
    pack $w -side top -fill x
    label $w.a -text $msg
    pack $w.a -side left

    if {[string length $date]} {
        label $w.b -text [clock format $date -format "%b %d %y %H:%M "]
    } else {
        label $w.b -text Never
    }
    pack $w.b -side right
}

proc entry-fieldsx {width parent list tlist returnAction escapeAction} {
    set alist {}
    set i 0
    foreach field $list {
        set label ${parent}.${field}.label
        set entry ${parent}.${field}.entry
        label $label -text [lindex $tlist $i]
        entry $entry -relief sunken -border 1 -width $width
        pack $label -side left
        pack $entry -side right
        lappend alist $entry
        incr i
    }
    bind-fields $alist $returnAction $escapeAction
}

proc protocol-setup {target} {
    global profileS profile
    
    foreach n [array names profile $target,*] {
	set profileS($n) $profile($n)
    }
    target-setup $target 0 0
}

proc protocol-setup-action {target} {
    global profileS profile settingsChanged
    
    set timedef $profileS($target,timeDefine)
    if {![string length $timedef]} {
        set timedef [clock seconds]
    }

    foreach n [array names profileS $target,*] {
	set profile($n) $profileS($n)
	unset profileS($n)
    }
    set settingsChanged 1

    cascade-target-list
    delete-target-hotlist $target
}

proc target-setup-delete {target category dir} {

    if {![string compare $target Default]} return
    set a [alert "Are you sure you want to delete the target \
definition $target ?"]
    if {$a} {
	target-setup $target $category $dir
    }
}

proc target-setup {target category dir} {
    global profile settingsChanged

    set w .setup-$profile($target,windowNumber)

    if {$dir} {
        target-setup-leave-$category $target $w
    }
    if {$dir == 3} {
	foreach n [array names profile $target,*] {
	    unset profile($n)
	}
	set settingsChanged 1
	cascade-target-list
	delete-target-hotlist $target
	destroy $w
	return
    } elseif {$dir == 2} {
        protocol-setup-action $target
        destroy $w
        return
    } else {
	incr category $dir
	if {[winfo exists $w]} {
	    destroy $w.top
	    destroy $w.bot
	} else {
	    toplevelG $w
	    wm geometry $w 430x370
	}
	if {$target == ""} {
	    set target Default
	}
	top-down-window $w
	bottom-buttons $w \
	    [list {Ok} [list target-setup $target $category 2] \
		 {Previous} [list target-setup $target $category -1] \
		 {Next} [list target-setup $target $category 1] \
		 {Delete} [list target-setup-delete $target $category 3] \
		 {Cancel} [list destroy $w]] 0
	if {$category == 0} {
	    $w.bot.2 configure -state disabled
	}
	if {$category == 2} {
	    $w.bot.4 configure -state disabled
	}
	target-setup-enter-$category $target $w
    }
}


proc target-setup-leave-0 {target w} {
    global profileS

    set y $w.top.hostport
}

proc target-setup-enter-0 {target w} {
    global profileS

    wm title $w "$target - Initial Information"

    # host/port/id . . .
    set y $w.top.hostport
    frame $y -relief ridge -border 2
    pack $y -padx 2 -pady 2 -side top -fill x
    frame $y.host
    frame $y.port
    frame $y.idAuthentication

    pack $y.host $y.port $y.idAuthentication -side top -fill x -pady 2

    entry-fieldsx 34 $y {host port idAuthentication} \
            {{Host:} {Port:} {Id Authentication:}} \
            [list target-setup $target 0 2] [list destroy $w]

    $y.host.entry configure -textvariable profileS($target,host)
    $y.port.entry configure -textvariable profileS($target,port)
    $y.idAuthentication.entry configure -textvariable profileS($target,idAuthentication)

    # databases

    frame $w.top.name -relief ridge -border 2
    pack  $w.top.name -pady 2 -padx 2 -side bottom -fill both -expand yes

	frame $w.top.name.add
	pack  $w.top.name.add -pady 1 -padx 2 -side top -fill both -expand yes

	label $w.top.name.add.label -text "Database to add:" -anchor e
    entry $w.top.name.add.entry -width 32 -relief sunken
	button $w.top.name.add.add -text "Add" -command \
		[list target-setup-db-add-action $target $w]
    pack $w.top.name.add.label $w.top.name.add.entry $w.top.name.add.add -side left

	label $w.top.name.label -text "Databases"
    pack $w.top.name.label -side top -fill x

    frame $w.top.name.buttons -border 2
    pack $w.top.name.buttons -side right

#    button $w.top.name.buttons.add -text "Add" -command \
#	[list target-setup-db-add $target $w]
    button $w.top.name.buttons.remove -text "Remove" -state disabled \
        -command [list target-setup-db-remove $target $w]
    button $w.top.name.buttons.configure -text "Configure" -state disabled
#    pack $w.top.name.buttons.add -side top -fill x
    pack $w.top.name.buttons.remove -side top -fill x
    pack $w.top.name.buttons.configure -side top -fill x

    scrollbar $w.top.name.scroll -orient vertical -border 1
    listbox $w.top.name.list -border 1 -height 5 -yscrollcommand \
	[list $w.top.name.scroll set] 
    pack $w.top.name.list -side left -padx 2 -pady 2 -fill both -expand yes
    pack $w.top.name.scroll -side right -padx 2 -pady 2 -fill y
    $w.top.name.scroll config -command [list $w.top.name.list yview]

    target-setup-dblist-update $target $w
    
    # misc. dates . . .

    set y $w.top.dates
    frame $y -relief ridge -border 2
    pack $y -pady 2 -padx 2 -side left -fill both -expand yes

    label $y.label -text "Dates"
    pack $y.label -side top -fill x
    print-date $w.top.dates.a {Defined:}      $profileS($target,timeDefine)
    print-date $w.top.dates.b {Last Access:}  $profileS($target,timeLastInit)
    print-date $w.top.dates.c {Last Explain:} $profileS($target,timeLastExplain)

    # protocol . . .

    set y $w.top.protocol

    frame $y -relief ridge -border 2
    pack $y -pady 2 -padx 2 -side right -fill both
    
    label $y.label -text "Protocol" 
    radiobutton $y.z39v2 -text "Z39.50" -anchor w \
            -variable profileS($target,protocol) -value Z39
    radiobutton $y.sr -text "SR" -anchor w \
            -variable profileS($target,protocol) -value SR
    pack $y.label $y.z39v2 $y.sr -padx 2 -side top -fill x

    # transport/comstack . . .

    set y $w.top.comstack
    frame $y -relief ridge -border 2
    pack $y -pady 2 -padx 2 -side right -fill both
    
    label $y.label -text "Transport" 
    radiobutton $y.tcpip -text "TCP/IP" -anchor w \
            -variable profileS($target,comstack) -value tcpip
    radiobutton $y.mosi -text "MOSI" -anchor w\
            -variable profileS($target,comstack) -value mosi
    pack $y.label $y.tcpip $y.mosi -padx 2 -side top -fill x

}

proc target-setup-leave-1 {target w} {
    global profileS

    set y $w.top.nr

    set profileS($target,targetInfoName) [string trim [$y.name.text get 0.0 end]]
    set profileS($target,recentNews) [string trim [$y.recentNews.text get 0.0 end]]
    set profileS($target,description) [string trim [$y.description.text get 0.0 end]]
    set profileS($target,welcomeMessage) [string trim [$y.welcome.text get 0.0 end]]

    set y $w.top.rs
}

proc target-setup-enter-1 {target w} {
    global profileS

    wm title $w "$target - Target Information"

    # Name, Recent News . . .
    set y $w.top.nr
    frame $y -relief ridge -border 2
    pack $y -side top -padx 2 -pady 2 -fill x
    
    frame $y.name
    frame $y.recentNews
    frame $y.description
    frame $y.welcome
    pack $y.name $y.recentNews $y.description $y.welcome \
            -side top -fill x -pady 2  -expand yes
    
    label $y.name.label -text "Name" -width 15
    pack $y.name.label -side left
    text $y.name.text -width 40 -height 2 -relief sunken -border 1 -wrap word
    TextEditable $y.name.text
    $y.name.text insert end $profileS($target,targetInfoName)
    pack $y.name.text -side right -fill x -expand yes
    
    label $y.recentNews.label -text "Recent News" -width 15
    pack $y.recentNews.label -side left
    text $y.recentNews.text -width 40 -height 2 -relief sunken -border 1 -wrap word
    TextEditable $y.recentNews.text
    $y.recentNews.text insert end $profileS($target,recentNews)
    pack $y.recentNews.text -side right -fill x -expand yes

    label $y.description.label -text "Description" -width 15
    pack $y.description.label -side left
    text $y.description.text -width 40 -height 4 -relief sunken -border 1 -wrap word
    TextEditable $y.description.text
    $y.description.text insert end $profileS($target,description)
    pack $y.description.text -side right -fill x -expand yes

    label $y.welcome.label -text "Welcome Message" -width 15
    pack $y.welcome.label -side left
    text $y.welcome.text -width 40 -height 4 -relief sunken -border 1 -wrap word
    TextEditable $y.welcome.text
    $y.welcome.text insert end $profileS($target,welcomeMessage)
    pack $y.welcome.text -side right -fill x -expand yes
    
    # Result Sets Size, numbers, etc. . . .
    set y $w.top.rs

    frame $y -relief ridge -border 2
    pack $y -side left -padx 2 -pady 2 -fill y

    frame $y.maxResultSets
    frame $y.maxResultSize
    frame $y.maxTerms
    pack $y.maxResultSets $y.maxResultSize $y.maxTerms -side top -fill x -pady 2
    
    entry-fieldsx 10 $y {maxResultSets maxResultSize maxTerms} \
            {{Max Result Sets:} {Max Result Size:} {Max Terms:}} \
            [list target-setup $target 1 2] [list destroy $w]

    $y.maxResultSets.entry configure -textvariable profileS($target,targetMaxResultSets)
    $y.maxResultSize.entry configure -textvariable profileS($target,targetMaxResultSize)
    $y.maxTerms.entry configure -textvariable profileS($target,targetMaxTerms)

    # Checkbuttons . . .
    set y $w.top.ns

    frame $y -relief ridge -border 2
    pack $y -side right -padx 2 -pady 2 -fill both -expand yes

    checkbutton $y.resultSets -text "Named Result Sets" \
            -anchor n -variable profileS($target,namedResultSets)  
    checkbutton $y.multipleDatabases -text "Multiple Database Search" \
            -anchor n -variable profileS($target,multipleDatabases)
    pack $y.resultSets $y.multipleDatabases -side top -padx 2 -pady 2

}

proc target-setup-2-dbselect {menu e} {
    $menu configure -text $e
}

proc target-setup-leave-2 {target w} {
    global profileS
}

proc target-setup-db-add {target wp} {
    set w .database-select
    toplevel $w
    set oldFocus [focus]
 
    place-force $w $wp
    top-down-window $w

    frame $w.top.database
    pack $w.top.database -side top -anchor e -pady 2
    
    entry-fields $w.top {database} {{Database to add:}} \
            [list target-setup-db-add-action $target $wp] [list destroy $w]
    top-down-ok-cancel $w [list target-setup-db-add-action $target $wp] 1
    focus $oldFocus
}

proc target-setup-db-add-action {target wp} {
    global profileS

#    set w .database-select
#	set db [$w.top.database.entry get]
    set db [$wp.top.name.add.entry get]
    lappend profileS($target,databases) $db

#    destroy $w
    target-setup-dblist-update $target $wp
}

proc target-setup-db-remove {target wp} {
    global profileS

    set w .setup100
    set y $w.top.name

    set db [$wp.top.name.list get active]

    set a [alert "Are you sure you want to remove the database ${db}?"]
    if {$a} {
        set i [lsearch -exact $profileS($target,databases) $db]
        if {$i >= 0} {
            set profileS($target,databases) \
                    [lreplace $profileS($target,databases) $i $i]
        }
        target-setup-dblist-update $target $wp
        if {![llength $profileS($target,databases)]} {
            unset profileS($target,databases)
        }
    }
}

proc target-setup-dblist-update {target w} {
    global profileS

    set y $w.top.name

    $w.top.name.list delete 0 end
    if {[info exists profileS($target,databases)]} {
        foreach db $profileS($target,databases) {
            $w.top.name.list insert end $db  
        }
        $w.top.name.buttons.remove configure -state normal
        $w.top.name.list see 0 
        $w.top.name.list select set 0
    } else {
        $w.top.name.buttons.remove configure -state disabled
    }
}

proc target-setup-add {target w} {


}

proc target-setup-enter-2 {target w} {
    global profileS

    wm title $w "$target - Other Information (not yet completed)"
   
    frame $w.top.data -relief ridge -border 2
    pack $w.top.data -pady 2 -padx 2 -side top -fill x

    frame $w.top.data.avRecordSize
    frame $w.top.data.maxRecordSize

    pack $w.top.data $w.top.data.avRecordSize $w.top.data.maxRecordSize \
		-side top -fill x -pady 2
    
    entry-fieldsx 14 $w.top.data \
	    {avRecordSize maxRecordSize} \
	    {{Average Record Size:} {Max Record Size:}} \
	    [list target-setup $target 2 2] [list destroy $w]
}

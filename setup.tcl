# IR toolkit for tcl/tk
# (c) Index Data 1995-1996
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: setup.tcl,v $
# Revision 1.1  1996-09-13 10:54:25  adam
# Started work on Explain in client.
#
#

set pref(font,h1) {-Adobe-Helvetica-Bold-R-Normal-*-240-*}
set pref(font,h2) {-Adobe-Helvetica-Bold-R-Normal-*-180-*}
set pref(font,h3) {-Adobe-Helvetica-Bold-R-Normal-*-140-*}
set pref(font,h4) {-Adobe-Helvetica-Bold-R-Normal-*-120-*}

set pref(font,s1) {-Adobe-Helvetica-Bold-R-Normal-*-100-*}
set pref(font,s2) {-Adobe-Helvetica-Bold-R-Normal-*-80-*}

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
    
    set tinfo $profile($target)

    set profileS($target,targetDescription) [lindex $tinfo 0]
    set profileS($target,host) [lindex $tinfo 1]
    set profileS($target,port) [lindex $tinfo 2]
    set profileS($target,idAuthentication) [lindex $tinfo 3]
    set profileS($target,targetMRS) [lindex $tinfo 4]

    set profileS($target,targetPMS) [lindex $tinfo 5]
    set profileS($target,comstack) [lindex $tinfo 6]
    set profileS($target,databases) [lindex $tinfo 7]
    set profileS($target,targetResultSets) [lindex $tinfo 8]
    set profileS($target,RPN) [lindex $tinfo 9]
    set profileS($target,CCL) [lindex $tinfo 10]

    set profileS($target,protocolType) [lindex $tinfo 11]
    set profileS($target,wno) [lindex $tinfo 12]
    set profileS($target,LSLB) [lindex $tinfo 13]
    set profileS($target,SSUB) [lindex $tinfo 14]

    set profileS($target,MSPN) [lindex $tinfo 15]
    set profileS($target,PresentChunk) [lindex $tinfo 16]
    set profileS($target,timeDefine) [lindex $tinfo 17]
    set profileS($target,timeInit) [lindex $tinfo 18]
    set profileS($target,timeExplain) [lindex $tinfo 19]

    set profileS($target,targetName) [lindex $tinfo 20]
    set profileS($target,targetRecentNews) [lindex $tinfo 21]
    set profileS($target,targetMaxResultSets) [lindex $tinfo 22]
    set profileS($target,targetMaxResultSize) [lindex $tinfo 23]
    set profileS($target,targetMaxTerms) [lindex $tinfo 24]

    set profileS($target,spare) [lindex $tinfo 25]
    set profileS($target,targetMultipleDatabases) [lindex $tinfo 26]
    set profileS($target,targetWelcome) [lindex $tinfo 27]

    target-setup $target 0 0
}

proc protocol-setup-action {target} {
    global profileS profile settingsChanged
    
    set timedef $profileS($target,timeDefine)
    if {![string length $timedef]} {
        set timedef [clock seconds]
    }
    set profile($target) [list \
            $profileS($target,targetDescription) \
            $profileS($target,host) \
            $profileS($target,port) \
            $profileS($target,idAuthentication) \
            $profileS($target,targetMRS) \
            $profileS($target,targetPMS) \
            $profileS($target,comstack) \
            $profileS($target,databases) \
            $profileS($target,targetResultSets) \
            $profileS($target,RPN) \
            $profileS($target,CCL) \
            $profileS($target,protocolType) \
            $profileS($target,wno) \
            $profileS($target,LSLB) \
            $profileS($target,SSUB) \
            $profileS($target,MSPN) \
            $profileS($target,PresentChunk) \
            $profileS($target,timeDefine) \
            $profileS($target,timeInit) \
            $profileS($target,timeExplain) \
            $profileS($target,targetName) \
            $profileS($target,targetRecentNews) \
            $profileS($target,targetMaxResultSets) \
            $profileS($target,targetMaxResultSize) \
            $profileS($target,targetMaxTerms) \
            $profileS($target,spare) \
            $profileS($target,targetMultipleDatabases) \
            $profileS($target,targetWelcome) \
            ]

    set settingsChanged 1

    cascade-target-list
    delete-target-hotlist $target
}

proc target-setup {target category dir} {

    set w .setup100
    if {$dir} {
        target-setup-leave-$category $target
    }
    if {$dir == 2} {
        protocol-setup-action $target
        destroy $w
        return
    }
    incr category $dir
    if {[winfo exists $w]} {
        destroy $w.top
        destroy $w.bot
    } else {
        toplevel $w
        wm geometry $w 430x400
    }
    if {$target == ""} {
        set target Default
    }
    top-down-window $w
    bottom-buttons $w [list \
            {Ok} [list target-setup $target $category 2] \
            {Previous} [list target-setup $target $category -1] \
            {Next} [list target-setup $target $category 1] \
            {Cancel} [list destroy $w]] 0
    if {$category == 0} {
        $w.bot.2 configure -state disabled
    }
    if {$category == 2} {
        $w.bot.4 configure -state disabled
    }
    target-setup-enter-$category $target
}


proc target-setup-leave-0 {target} {
    global profileS

    set w .setup100
    set y $w.top.hostport

    set profileS($target,host) [$y.host.entry get]
    set profileS($target,port) [$y.port.entry get]
    set profileS($target,idAuthentication) [$y.idAuthentication.entry get]
}

proc target-setup-enter-0 {target} {
    global profileS

    set w .setup100

    wm title $w "$target - Initial Information"

    # host/port/id . . .
    set y $w.top.hostport
    frame $y -relief ridge -border 2
    pack $y -padx 2 -pady 2 -side top -fill x
    frame $y.host
    frame $y.port
    frame $y.idAuthentication

    pack $y.host $y.port $y.idAuthentication -side top -fill x -pady 2

    entry-fieldsx 34 $y \
            {host port idAuthentication} \
            {{Host:} {Port:} {Id Authentication:}} \
            [list target-setup $target 0 2] [list destroy $w]

    $y.host.entry insert 0 $profileS($target,host)
    $y.port.entry insert 0 $profileS($target,port)
    $y.idAuthentication.entry insert 0 $profileS($target,idAuthentication)

    # bottom

    set y $w.top.bottom

    frame $y
    pack $y -side bottom -fill both -expand yes
    
    # misc. dates . . .

    set y $w.top.dates
    frame $y -relief ridge -border 2
    pack $y -pady 2 -padx 2 -side left -fill both -expand yes

    label $y.label -text "Dates"
    pack $y.label -side top -fill x
    print-date $w.top.dates.a {Defined:}      $profileS($target,timeDefine)
    print-date $w.top.dates.b {Last Access:}  $profileS($target,timeInit)
    print-date $w.top.dates.c {Last Explain:} $profileS($target,timeExplain)

    # protocol . . .

    set y $w.top.protocol

    frame $y -relief ridge -border 2
    pack $y -pady 2 -padx 2 -side right -fill both
    
    label $y.label -text "Protocol" 
    radiobutton $y.z39v2 -text "Z39.50" -anchor w \
            -variable profileS($target,protocolType) -value Z39
    radiobutton $y.sr -text "SR" -anchor w \
            -variable profileS($target,protocolType) -value SR
    
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

proc target-setup-leave-1 {target} {
    global profileS

    set w .setup100
    set y $w.top.nr

    set profileS($target,targetName) [$y.name.text get 0.0 end]
    set profileS($target,targetRecentNews) [$y.recentNews.text get 0.0 end]
    set profileS($target,targetDescription) [$y.description.text get 0.0 end]

    set y $w.top.rs

    set profileS($target,targetMaxResultSets) [$y.maxResultSets.entry get]
    set profileS($target,targetMaxResultSize) [$y.maxResultSize.entry get]
    set profileS($target,targetMaxTerms) [$y.maxTerms.entry get]
}

proc target-setup-enter-1 {target} {
    global profileS

    set w .setup100

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
    text $y.name.text -width 40 -height 2 -relief sunken -border 1 \
            -wrap word
    TextEditable $y.name.text
    $y.name.text insert end $profileS($target,targetName)
    pack $y.name.text -side right -fill x -expand yes
    
    label $y.recentNews.label -text "Recent News" -width 15
    pack $y.recentNews.label -side left
    text $y.recentNews.text -width 40 -height 2 -relief sunken -border 1 \
            -wrap word
    TextEditable $y.recentNews.text
    $y.recentNews.text insert end $profileS($target,targetRecentNews)
    pack $y.recentNews.text -side right -fill x -expand yes

    label $y.description.label -text "Description" -width 15
    pack $y.description.label -side left
    text $y.description.text -width 40 -height 4 -relief sunken -border 1 \
            -wrap word
    TextEditable $y.description.text
    $y.description.text insert end $profileS($target,targetDescription)
    pack $y.description.text -side right -fill x -expand yes

    label $y.welcome.label -text "Welcome Message" -width 15
    pack $y.welcome.label -side left
    text $y.welcome.text -width 40 -height 4 -relief sunken -border 1 \
            -wrap word
    TextEditable $y.welcome.text
    $y.welcome.text insert end $profileS($target,targetWelcome)
    pack $y.welcome.text -side right -fill x -expand yes
    
    # Result Sets Size, numbers, etc. . . .
    set y $w.top.rs

    frame $y -relief ridge -border 2
    pack $y -side left -padx 2 -pady 2 -fill y

    frame $y.maxResultSets
    frame $y.maxResultSize
    frame $y.maxTerms

    pack $y.maxResultSets $y.maxResultSize $y.maxTerms \
            -side top -fill x -pady 2
    
    entry-fieldsx 10 $y \
            {maxResultSets maxResultSize maxTerms} \
            {{Max Result Sets:} {Max Result Size:} {Max Terms:}} \
            [list target-setup $target 1 2] [list destroy $w]

    $y.maxResultSets.entry insert 0 $profileS($target,targetMaxResultSets)
    $y.maxResultSize.entry insert 0 $profileS($target,targetMaxResultSize)
    $y.maxTerms.entry insert 0 $profileS($target,targetMaxTerms)

    # Checkbuttons . . .
    set y $w.top.ns

    frame $y -relief ridge -border 2
    pack $y -side right -padx 2 -pady 2 -fill both -expand yes

    checkbutton $y.resultSets -text "Named Result Sets" \
            -anchor n -variable profileS($target,targetResultSets)
    
    checkbutton $y.multipleDatabases -text "Multiple Database Search" \
            -anchor n -variable profileS($target,targetMultipleDatabases)

    pack $y.resultSets $y.multipleDatabases -side top -padx 2 -pady 2

}

proc target-setup-2-dbselect {menu e} {
    $menu configure -text $e
}

proc target-setup-leave-2 {target} {
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
    
    entry-fields $w.top {database} \
            {{Database to add:}} \
            [list target-setup-db-add-action $target $wp] \
            [list destroy $w]
    
    top-down-ok-cancel $w [list target-setup-db-add-action $target $wp] 1
    focus $oldFocus
}

proc target-setup-db-add-action {target wp} {
    global profileS

    set w .database-select

    set db [$w.top.database.entry get]
    if {![string length [lindex $profileS($target,databases) 0]]} {
        set profileS($target,databases) $db
    } else {
        lappend profileS($target,databases) $db
    }
    destroy $w
    target-setup-dblist-update $target
}

proc target-setup-db-remove {target wp} {
    global profileS

    set w .setup100
    set y $w.top.name

    set db [$y.data cget -text]
    set a [alert "Are you sure you want to remove the database ${db}?"]
    if {$a} {
        set i [lsearch -exact $profileS($target,databases) $db]
        if {$i >= 0} {
            set profileS($target,databases) \
                    [lreplace $profileS($target,databases) $i $i]
        }
        target-setup-dblist-update $target
    }
}

proc target-setup-dblist-update {target} {
    global profileS

    set w .setup100
    set y $w.top.name

    set no 0
    set databaseList $profileS($target,databases)
    $y.data configure -text [lindex $databaseList 0]
    $y.data.m delete 0 100
    foreach d $databaseList {
        $y.data.m add command -label $d -command \
                [list target-setup-2-dbselect $y.data $d]
        incr no
    }
    if {$no == 0} {
        $y.remove configure -state disabled
    } else {
        $y.remove configure -state normal
    }
}

proc target-setup-enter-2 {target} {
    global profileS

    set w .setup100

    set databaseList $profileS($target,databases)
    
    wm title $w "$target - Database Information"
    
    frame $w.top.name -border 2
    pack $w.top.name -pady 2 -padx 2 -side top -fill x
    
    label $w.top.name.label -text "Database Name" 
    
    pack $w.top.name.label -side left
    menubutton $w.top.name.data -menu $w.top.name.data.m -relief raised
    menu $w.top.name.data.m

    pack $w.top.name.data -side left
   
    button $w.top.name.add -text "Add" -command \
            [list target-setup-db-add $target $w]
    pack $w.top.name.add -side right

    button $w.top.name.remove -text "Remove" -command \
            [list target-setup-db-remove $target $w]
    pack $w.top.name.remove -side right

    frame $w.top.data -relief ridge -border 2
    pack $w.top.data -pady 2 -padx 2 -side top -fill x

    target-setup-dblist-update $target

    frame $w.top.data.avRecordSize
    frame $w.top.data.maxRecordSize

    pack $w.top.data $w.top.data.avRecordSize $w.top.data.maxRecordSize \
            -side top -fill x -pady 2
    
    entry-fieldsx 14 $w.top.data \
            {avRecordSize maxRecordSize} \
            {{Average Record Size:} {Max Record Size:}} \
            [list target-setup $target 2 2] [list destroy $w]
}

#!/usr/bin/wish
# $Id: client.tcl,v 1.8 1999-03-30 15:02:40 perhans Exp $
#

wm title . "IrTcl Client"

# Procedure irmenu
proc irmenu {w} {
    menu $w -tearoff off
}

proc debug-window {text} {
    if {[winfo exists .debug-window.top.t]} {
        .debug-window.top.t insert end "$text \n"
    } else {
        set w .debug-window
        toplevel $w
    
        wm title $w "Debug Window" 
        
        frame $w.top -relief raised -border 1
        frame $w.bot -relief raised -border 1
        pack  $w.top -side top -fill both -expand yes
        pack  $w.bot -fill both
        scrollbar $w.top.s -command [list $w.top.t yview]
        text $w.top.t -width 60 -height 10 -wrap word -relief flat \
            -borderwidth 0 -font fixed -yscroll [list $w.top.s set]
        pack $w.top.s -side right -fill y
        pack $w.top.t -expand yes -fill both -expand y
        .debug-window.top.t insert end "$text \n"
    }
}
    
# Procedure configure-enable-e {w n}
#  w   is a menu
#  n   menu entry number (0 is first entry)
# Enables menu entry
proc configure-enable-e {w n} {
    $w entryconfigure $n -state normal
}

# Procedure configure-disable-e {w n}
#  w   is a menu
#  n   menu entry number (0 is first entry)
# Disables menu entry
proc configure-disable-e {w n} {
    $w entryconfigure $n -state disabled
}
set noFocus [list -takefocus 0]


# Define dummy clock function if it is not there.
if {[catch {clock seconds}]} {
    proc clock {args} {
        return {}
    }
}

# Define libdir to the IrTcl configuration directory.
# In the line below LIBDIR will be modified during 'make install'.
set libdir LIBDIR

# If the bitmaps sub directory is present with a bitmap we assume 
# the client is run from the source directory in which case we
# set libdir the current directory.
if {[file readable [file join bitmaps book2]]} {
    set libdir .
}

# Make a final check to see if libdir was set ok.
if {! [file readable [file join $libdir bitmaps book2]]} {
    puts "Cannot locate system files in ${libdir}. You must either run this"
    puts "program from the source directory root of ir-tcl or you must assure"
    puts "that it is installed - normally in /usr/local/lib/irtcl"
    exit 1
}

# Initialize a lot of globals.
set hotTargets {}
set hotInfo {}
set busy 0

set profile(Default,description) {}
set profile(Default,host) {}
set profile(Default,port) 210
set profile(Default,authentication) {}
set profile(Default,maximumRecordSize) 50000
set profile(Default,preferredMessageSize) 30000
set profile(Default,comstack) tcpip
set profile(Default,namedResultSets) 1
set profile(Default,queryRPN) 1
set profile(Default,queryCCL) 0
set profile(Default,protocol) Z39
set profile(Default,windowNumber) 1
set profile(Default,largeSetLowerBound) 2
set profile(Default,smallSetUpperBound) 0
set profile(Default,mediumSetPresentNumber) 0
set profile(Default,presentChunk) 4
set profile(Default,timeDefine) {}
set profile(Default,timeLastInit) {}
set profile(Default,timeLastExplain) {}
set profile(Default,targetInfoName) {}
set profile(Default,recentNews) {}
set profile(Default,maxResultSets) {}
set profile(Default,maxResultSize) {}
set profile(Default,maxTerms) {}
set profile(Default,multipleDatabases) 0
set profile(Default,welcomeMessage) {}

set hostid Default
set currentDb Default
set settingsChanged 0
set setNo 0
set setNoLast 0
set cancelFlag 0
set scanEnable 0
set displayFormat 1
set popupMarcdf 0
set textWrap word
set recordSyntax None
set elementSetNames None
set delayRequest {}
set debugMode 0
set queryAutoOld 0
set leadText 1

set queryTypes {Simple}
set queryButtonsBib1 { { {I 0} {I 1} {I 2} } }
set queryInfoBib1 { { {Title {1=4 4=1}} {Author {1=1}} \
        {Subject {1=21}} {Any {1=1016}} } }
set queryAuto 1
wm minsize . 0 0

set setOffset 0
set setMax 0

set syntaxList {None sep USMARC UNIMARC UKMARC DANMARC FINMARC NORMARC \
    PICAMARC sep SUTRS sep GRS1}


set font(bb,normal) {Helvetica 24}
set font(bb,bold) {Helvetica 24 bold}
set font(b,normal) {Helvetica 24}
set font(b,bold) {Helvetica 18 bold}
set font(n,normal) {Helvetica 12}
set font(n,bold) {Helvetica 12 bold}
set font(s,bold) {Helvetica 10 bold}
set font(ss,bold) {Helvetica 8 bold}

# Procedure tkerror {err}
#   err   error message
# Override the Tk error handler function.
if {1} {
    proc tkerror err {
        global font
        set w .tkerrorw
        
        if {[winfo exists $w]} {
            destroy $w
        }
        toplevel $w
        wm title $w "Error"
        
        place-force $w .
        top-down-window $w
        
        label $w.top.b -bitmap error
        message $w.top.t -aspect 300 -text "Error: $err" -font $font(b,bold)
        pack $w.top.b $w.top.t -side left -padx 10 -pady 10
        
        bottom-buttons $w [list {Close} [list destroy $w]] 1
    }
}

# Read tag set file (if present)
if {[file readable [file join $libdir tagsets.tcl]]} {
    source [file join $libdir tagsets.tcl]
}

# Read the global target configuration file.
if {[file readable [file join $libdir irtdb.tcl]]} {
    source [file join $libdir irtdb.tcl]
}
# Read the local target configuration file.
if {[file readable "irtdb.tcl"]} {
    source "irtdb.tcl"
}

# Read the user configuration file.
if {[file readable "~/.clientrc.tcl"]} {
    source "~/.clientrc.tcl"
}

set queryAutoOld $queryAuto
set attribute$attributeTypeSelected 1

# Convert old format to new format...
foreach target [array names profile] {
    set timedef [clock seconds]
    if {[string first , $target] == -1} {
    if {![info exists profile($target,port)]} {
        foreach n [array names profile Default,*] {
            set profile($target,[string range $n 8 end]) $profile($n)
        }
        set profile($target,description) [lindex $profile($target) 0]
        set profile($target,host) [lindex $profile($target) 1]
        set profile($target,port) [lindex $profile($target) 2]
        set profile($target,authentication) [lindex $profile($target) 3]
        set profile($target,maximumRecordSize) \
        [lindex $profile($target) 4]
        set profile($target,preferredMessageSize) \
        [lindex $profile($target) 5]
        set profile($target,comstack) [lindex $profile($target) 6]
        set profile($target,databases) [lindex $profile($target) 7]
        set profile($target,timeDefine) $timedef
        set profile($target,windowNumber) 1
    }
    unset profile($target)
    }
}

# Assign unique ID's to each target's window number
set wno 1
foreach n [array names profile *,windowNumber] {
    set profile($n) $wno
    incr wno
}
set profile(Default,windowNumber) $wno

# These globals describe the current query type. They are set to the
# first query type.
if {[info exists queryButtons$attributeTypeSelected]} {
    update
    set queryButtonsFind [lindex [set "queryButtons$attributeTypeSelected"] 0]
} else {
    set queryButtonsFind [lindex [set queryButtonsBib1] 0]
    set queryButtons$attributeTypeSelected $queryButtonsBib1
} 
if {[info exists queryInfo$attributeTypeSelected]} {   
    set queryInfoFind [lindex [set "queryInfo$attributeTypeSelected"] 0]
} else {
    set queryInfoFind [lindex [set queryInfoBib1] 0]
    set queryInfo$attributeTypeSelected $queryInfoBib1
}    

# Procedure read-formats
# Read all Tcl source files in the subdirectory 'formats'.
# The name of each source will correspond to a display format.
proc read-formats {} {
    global displayFormats libdir

    set oldDir [pwd]
    cd [file join $libdir formats]
    set formats [glob {*.[tT][cC][lL]}]
    foreach f $formats {
        if {[file readable $f]} {
            source $f
            set l [string length $f]
            set f [string tolower [string range $f 0 [expr $l - 5]]]
            lappend displayFormats $f
        }
    }
    cd $oldDir
}

# Procedure set-wrap {m}
#  m    boolean wrap mode
# Handler to enable/disable text wrap in the main record window
proc set-wrap {m} {
    global textWrap

    set textWrap $m
    .data.record configure -wrap $m
}

# Procedure dputs {m}
#  m    string to be printed
# puts utility for debugging.
proc dputs {m} {
    global debugMode
    if {$debugMode == 1} {
        puts $m
    }
}

# Procedure apduDump {}
# Logs BER dump of last APDU in window if debugMode is true.
proc apduDump {} {
    global debugMode

    set w .apdu

    if {$debugMode == 0} return
    set x [z39 apduInfo]

    set offset [lindex $x 1]
    set length [lindex $x 0]

    if {![winfo exists $w]} {
        catch {destroy $w}
        toplevelG $w

        wm title $w "APDU information"       
        wm minsize $w 0 0
        
        top-down-window $w
        
        text $w.top.t -font fixed -width 60 -height 12 -wrap word \
            -relief flat -borderwidth 0 \
            -yscrollcommand [list $w.top.s set] -background grey85
        scrollbar $w.top.s -command [list $w.top.t yview]
        pack $w.top.s -side right -fill y
        pack $w.top.t -expand yes -fill both

        bottom-buttons $w [list {Close} [list destroy $w]] 0
    }
    $w.top.t insert end "Length: ${length}\n"
    if {$offset != -1} {
        $w.top.t insert end "Offset: ${offset}\n"
    }
    $w.top.t insert end [lindex $x 2]
    $w.top.t insert end "---------------------------------\n"

}

# Procedure set-display-format {f}
#  f    display format
# Reformats main record window to use display format given by f
proc set-display-format {f} {
    global displayFormat setNo busy

    set displayFormat $f
    if {$setNo == 0} {
        return
    }
    if {!$busy} {
        .bot.a.status configure -text "Reformatting"
    }
    update idletasks
    add-title-lines -1 10000 1
}

# Procedure initBindings
# Disables various default bindings for Text and Listbox widgets.
proc initBindings {} {
    global TextBinding

    foreach e [bind Text] {
        set TextBinding($e) [bind Text $e]
        bind Text $e {}
    }
    set w Listbox
    bind $w <B1-Motion> {}
    bind $w <Shift-B1-Motion> {}

    set w Entry
}

# Procedure TextEditable 
# Apply "standard" events to a text widget. It should be editable now.
proc TextEditable {w} {
    global TextBinding

    foreach e [array names TextBinding] {
        bind $w $e $TextBinding($e)
    }
}

# Procedure destroyGW {w}
#   w     top level widget
# Saves geometry of widget w in windowGeometry array. This
# Procedure is used to save current geometry of a window before
# it is destroyed.
# See also topLevelG.
proc destroyGW {w} {
    global windowGeometry
    catch {set windowGeometry($w) [wm geometry $w]}
}    

# Procedure topLevelG
#   w     top level widget
# Makes a new top level widget named w; sets geometry of window if it 
# exists in windowGeometry array. The destroyGW procedure is set 
# to be called when the Destroy event occurs.
proc toplevelG {w} {
    global windowGeometry

    if {![winfo exists $w]} {
        toplevel $w
    }
    if {[info exists windowGeometry($w)]} {
        set g $windowGeometry($w)
        if {$g != ""} {
            wm geometry $w $g
        }
    } 
    bind $w <Destroy> [list destroyGW $w]
}

# Procedure top-down-window {w}
#  w    window (possibly top level)
# Makes two frames inside w called top and bot.
proc top-down-window {w} {
    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    pack  $w.top -side top -fill both -expand yes
    pack  $w.bot -fill both
}

# Procedure top-down-ok-cancel {w ok-action g}
#  w          top level widget with $w.bot-frame
#  ok-action  ok script
#  g          grab flag
# Makes two buttons in the bot frame called Ok and Cancel. The
# ok-action is executed if Ok is pressed. If Cancel is activated
# The window is destroyed. If g is true a grab is performed on the
# window and the procedure waits until the window is destroyed.
proc top-down-ok-cancel {w ok-action g} {
    frame $w.bot.left -relief sunken -border 1
    pack $w.bot.left -side left -expand yes -ipadx 2 -ipady 2 -padx 1 -pady 1
    button $w.bot.left.ok -width 4 -text {Ok} -command ${ok-action}
    pack $w.bot.left.ok -expand yes -ipadx 1 -ipady 1 -padx 2 -pady 2
    button $w.bot.cancel -width 5 -text {Cancel} -command [list destroy $w]
    pack $w.bot.cancel -side left -expand yes    

    if {$g} {
        grab $w
        tkwait window $w
    }
}

# Procedure bottom-buttons {w buttonList g}
#  w          top level widget with $w.bot-frame
#  buttonList button specifications
#  g          grab flag
# Makes a list of buttons in the $w.bot frame. The buttonList is a list 
# of button specifications. Each button specification consists of two
# items; the first item is button label name; the second item is a script
# of be executed when that button is executed. A grab is performed if g 
# is true and it waits for the window to be destroyed.
proc bottom-buttons {w buttonList g} {
    set i 0
    set l [llength $buttonList]

    frame $w.bot.$i -relief sunken -border 1
    pack $w.bot.$i -side left -expand yes -padx 2 -pady 2
    button $w.bot.$i.ok -text [lindex $buttonList $i] \
        -command [lindex $buttonList [expr $i + 1]]
    pack $w.bot.$i.ok -expand yes -padx 2 -pady 2 -side left

    incr i 2
    while {$i < $l} {
        button $w.bot.$i -text [lindex $buttonList $i] \
                -command [lindex $buttonList [expr $i + 1]]
        pack $w.bot.$i -expand yes -padx 2 -pady 2 -side left
        incr i 2
    }
    if {$g} {
        # Grab ...
        grab $w
        tkwait window $w
    }
}

# Procedure cancel-operation
# This handler is invoked when the user wishes to cancel an operation.
# If the system is currently busy a "Cancel" will be displayed in the
# status area and the cancelFlag is set to true indicating that future
# responses from the target should be ignored. The system is no longer
# busy when this procedure exists.
proc cancel-operation {} {
    global cancelFlag busy delayRequest

    if {$busy} {
        set cancelFlag 1
        set delayRequest {}
        show-status Cancel 0 1
    }
}

# Procedure show-target {target base}
#  target     name of target
#  base       name of database
# Displays target name and database name in the target status area.
proc show-target {target base} {
    if {![string length $target]} {
        .bot.a.target configure -text {}
        return
    }
    if {![string length $base]} {
        .bot.a.target configure -text "$target"
    } else {
         .bot.a.target configure -text "$target - $base"
    }
}

# Procedure show-logo {v1}
#  v1    integer level
# This procedure maintains the book logo in the bottom of the screen.
# It is invoked only once during initialization of windows, etc., and
# by itself. The global 'busy' variable determines whether the logo is
# moving or not.
proc show-logo {v1} {
    global busy libdir

    if {$busy != 0} {
        incr v1
        if {$v1==10} {
            set v1 1
        }
        .bot.logo configure -bitmap @[file join $libdir bitmaps book${v1}] 
        after 140 [list show-logo $v1]
        return
    }
    while {1} {
        .bot.logo configure -bitmap @[file join $libdir bitmaps book1]
        tkwait variable busy
        if {$busy} {
            show-logo 1
            return
        }
    }
}

# Procedure show-status {status b sb}
#  status     status message string
#  b          busy indicator
#  sb         search busy indicator
# Display status information according to 'status' and sets the global
# busy flag 'busy' to b if b is non-empty. If sb is non-empty it indicates
# whether service buttons should be enabled or disabled.
proc show-status {status b sb} {
    global busy scanEnable setOffset setMax setNo

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
        if {$scanEnable} {
            .mid.scan configure -state normal
        } else {
            configure-disable-e .top.service.m 3
        }
        if {$setNo == 0} {
            configure-disable-e .top.service.m 1
        } elseif {[z39.$setNo nextResultSetPosition] > 0 && 
            [z39.$setNo nextResultSetPosition] <= [z39.$setNo resultCount]} {
            configure-enable-e .top.service.m 1
            .mid.present configure -state normal
        } else {
            configure-disable-e .top.service.m 1
            .mid.present configure -state disabled
        }
        if {[winfo exists .scan-window]} {
            .scan-window.bot.2 configure -state normal
            .scan-window.bot.4 configure -state normal
        }
    } else {
        .top.service configure -state disabled
        .mid.search configure -state disabled
        .mid.scan configure -state disabled
        .mid.present configure -state disabled

        if {[winfo exists .scan-window]} {
            .scan-window.bot.2 configure -state disabled
            .scan-window.bot.4 configure -state disabled
        }
    }
}

# Procedure show-message {msg}
#  msg    message string
# Sets message the bottom of the screen to msg.
proc show-message {msg} {
    .bot.a.message configure -text "$msg"
}

# Procedure insertWithTags {w text args}
#  w      text widget
#  text   string to be inserted
#  args   list of tags
# Inserts text at the insertion point in widget w. The text is tagged 
# with the tags in args.
proc insertWithTags {w text args} {
    global leadText
    set text [string trimright $text ,]
    set start [$w index insert]
    if {[lsearch {marc-text marc-it marc-id} $args] == -1||$leadText} {
        $w insert insert "$text"
    } else {
        $w insert insert ", $text"
    }
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start insert
    }
    foreach i $args {
        $w tag add $i $start insert
    }
    set leadText 0
    if {[lsearch -exact {marc-head marc-pref marc-tag marc-small-head} $args] != -1} {
        set leadText 1
    }
}

# Procedure popup-license and displays LICENSE information.
proc popup-license {} {
    global libdir
    set w .popup-license
    toplevel $w

    wm title $w "License" 
    wm minsize $w 0 0

    top-down-window $w

    text $w.top.t -width 80 -height 10 -wrap word -relief flat -borderwidth 0 \
        -font fixed -yscrollcommand [list $w.top.s set]
    scrollbar $w.top.s -command [list $w.top.t yview]
    pack $w.top.s -side right -fill y
    pack $w.top.t -expand yes -fill both

    if {[file readable [file join $libdir LICENSE]]} {
        set f [open [file join $libdir LICENSE] r]
        while {[gets $f buf] != -1} {
            $w.top.t insert end $buf
            $w.top.t insert end "\n"
        } 
        close $f
    }
    bottom-buttons $w [list {Close} [list destroy $w]] 1
}

# Procedure about-target
# Displays various information about the current target, such
# as implementation-name, implementation-id, etc.
proc about-target {} {
    set w .about-target-w
    global hostid font

    toplevel $w

    wm title $w "About target"
    place-force $w .
    top-down-window $w

    frame $w.top.a -relief ridge -border 2
    frame $w.top.p -relief ridge -border 2 -background white
    pack $w.top.a $w.top.p -side top -fill x

    label $w.top.a.about -text "About"
    label $w.top.a.irtcl -text $hostid -font $font(bb,bold)
    pack $w.top.a.about $w.top.a.irtcl -side top

    set i [z39 targetImplementationName]
    label $w.top.p.in -text "Implementation name: $i" -background white
    set i [z39 targetImplementationId]
    label $w.top.p.ii -text "Implementation id: $i" -background white
    set i [z39 targetImplementationVersion]
    label $w.top.p.iv -text "Implementation version: $i" -background white
    set i [z39 options]
    label $w.top.p.op -text "Protocol options: $i" -background white
    pack $w.top.p.in $w.top.p.ii $w.top.p.iv $w.top.p.op -side top -anchor nw

    bottom-buttons $w [list {Close} [list destroy $w]] 1
}

# Procedure about-origin-logo {n}
#   n    integer level
# Displays book logo in the .about-origin-w widget
proc about-origin-logo {n} {
    global libdir
    set w .about-origin-w
    if {![winfo exists $w]} {
        return
    }
    incr n
    if {$n==10} {
        set n 1
    }
    $w.top.a.logo configure -bitmap @[file join $libdir bitmaps book$n]
    after 140 [list about-origin-logo $n]
}

# Procedure about-origin
# Display various information about origin (this client).
proc about-origin {} {
    set w .about-origin-w
    global libdir font tk_version
    
    if {[winfo exists $w]} {
        destroy $w
    }
    toplevel $w

    wm title $w "About IrTcl"
    place-force $w .
    top-down-window $w

    frame $w.top.a -relief ridge -border 2
    frame $w.top.p -relief ridge -border 2
    pack $w.top.a $w.top.p -side top -fill x
    
    label $w.top.a.irtcl -text "IrTcl" -font $font(bb,bold)
    label $w.top.a.logo -bitmap @[file join $libdir bitmaps book1] 
    pack $w.top.a.irtcl $w.top.a.logo -side left -expand yes

    set i unknown
    catch {set i [z39 implementationName]}
    label $w.top.p.in -text "Implementation name: $i"
    catch {set i [z39 implementationId]}
    label $w.top.p.ii -text "Implementation id: $i"
    catch {set i [z39 implementationVersion]}
    label $w.top.p.iv -text "Implementation version: $i"
    set i $tk_version
    label $w.top.p.tk -text "Tk version: $i"
    pack $w.top.p.in $w.top.p.ii $w.top.p.iv $w.top.p.tk -side top -anchor nw

    about-origin-logo 1
    bottom-buttons $w [list {Close} [list destroy $w] \
                            {License} [list popup-license]] 0
}

# Procedure popup-marc {sno no b df}
#  sno     result set number
#  no      record position number
#  b       popup window number
#  df      display format
# Displays record in set $sno at position $no in window .full-marc$b.
# The global variable $popupMarcdf holds the current format method.
proc popup-marc {sno no b df} {
    global font displayFormats popupMarcdf

    if {[z39.$sno type $no] != "DB"} {
        return
    }
    if {$b == -1} {
        set b 0
        while {[winfo exists .full-marc$b]} {
            incr b
        }
    }
    set df $popupMarcdf
    set w .full-marc$b
    if {![winfo exists $w]} {
        toplevelG $w

        wm minsize $w 0 0
        
        frame $w.top -relief raised -border 1
        frame $w.bot -relief raised -border 1
        pack  $w.top -side top -fill both -expand yes
        pack  $w.bot -fill both

        text $w.top.record -width 60 -height 5 -wrap word -relief flat \
            -borderwidth 0 -font fixed \
            -yscrollcommand [list $w.top.s set] -background white
        scrollbar $w.top.s -command [list $w.top.record yview] 
        $w.top.record tag configure marc-tag -foreground blue
        $w.top.record tag configure marc-id -foreground red
        $w.top.record tag configure marc-data -foreground black
        $w.top.record tag configure marc-head -font $font(n,bold) \
             -background black -foreground white
        $w.top.record tag configure marc-pref -font $font(n,normal) -foreground blue
        $w.top.record tag configure marc-text -font $font(n,normal) -foreground black
        $w.top.record tag configure marc-it -font $font(n,normal) -foreground black

        pack $w.top.s -side right -fill y
        pack $w.top.record -expand yes -fill both
        
        bottom-buttons $w [list {Close} [list destroy $w] \
            {Prev} {} {Next} {} {Duplicate} {}] 0
        menubutton $w.bot.formats -text "Format" -menu $w.bot.formats.m -relief raised
        irmenu $w.bot.formats.m
        pack $w.bot.formats -expand yes -ipadx 2 -ipady 2 -padx 3 -pady 3 -side left
    } else {
        $w.bot.formats.m delete 0 last
    }
    set i 0
    foreach f $displayFormats {
        $w.bot.formats.m add radiobutton -label $f -variable popupMarcdf -value $i \
            -command [list popup-marc $sno $no $b 0]
        incr i
    }
    $w.top.record delete 0.0 end
    set recordType [z39.$sno recordType $no]
    wm title $w "$recordType record #$no"
    focus $w

    $w.bot.2 configure -command [list popup-marc $sno [expr $no-1] $b $df]
    $w.bot.4 configure -command [list popup-marc $sno [expr $no+1] $b $df]
    if {$no == 1} {
        $w.bot.2 configure -state disabled
    } else {
        $w.bot.2 configure -state normal
    }
    if {[z39.$sno type [expr $no+1]] != "DB"} {
        $w.bot.4 configure -state disabled
    } else {
        $w.bot.4 configure -state normal
    }
    $w.bot.6 configure -command [list popup-marc $sno $no -1 0]
    set ffunc [lindex $displayFormats $df]
    set ffunc "display-$ffunc"

    $ffunc $sno $no $w.top.record 0
}

# Procedure update-target-hotlist {target base}
#  target     current target name
#  base       current database name
# Updates the global $hotTargets so that $target and $base are
# moved to the front, i.e. they become the number 1 target/base.
# The target menu is updated by a call to set-target-hotlist.
proc update-target-hotlist {target base} {
    global hotTargets

    set olen [llength $hotTargets]
    set i 0
    foreach e $hotTargets {
        if {$target == [lindex $e 0] && $base == [lindex $e 1]} {
            set hotTargets [lreplace $hotTargets $i $i]
            break
        }
        incr i    
    }
    set hotTargets [linsert $hotTargets 0 [list $target $base]]
    set-target-hotlist $olen
} 

# Procedure delete-target-hotlist {target}
#  target    target to be deleted
# Updates the global $hotTargets so that $target is removed.
# The target menu is updated by a call to set-target-hotlist.
proc delete-target-hotlist {target} {
    global hotTargets

    set olen [llength $hotTargets]
    set i 0
    foreach e $hotTargets {
        if {$target == [lindex $e 0]} {
        set hotTargets [lreplace $hotTargets $i $i]
        }
        incr i
    }
    set-target-hotlist $olen
}

# Procedure set-target-hotlist {olen}
#  olen     number of hot target entries to be deleted from menu
# Updates the target menu with the targets with the first 8 entries
# in the $hotTargets global.
proc set-target-hotlist {olen} {
    global hotTargets
   
    if {$olen > 0} {
       .top.target.m delete 6 [expr 6+$olen]
    }
    set i 1
    foreach e $hotTargets {
        set target [lindex $e 0]
        set base [lindex $e 1]
        if {![string length $base]} {
            .top.target.m add command -label "$i $target" -command \
                [list reopen-target $target {}]
        } else {
            .top.target.m add command -label "$i $target - $base" -command \
                [list reopen-target $target $base]
        }
        incr i
        if {$i > 8} {
             break
        }
    }
}

# Procedure reopen-target {target base}
#  target    target to be opened
#  base      base to be used
# Closes connection with current target and opens a new connection
# with $target and database $base.
proc reopen-target {target base} {
    close-target
    open-target $target $base
    update-target-hotlist $target $base
}

# Procedure define-target-action
# Prepares the setup of a new target. The name of the target
# is read from the dialog .target-define dialog (procedure
# define-target-dialog) and the target definition window is displayed by
# a call to protocol-setup.
proc define-target-action {} {
    global profile
    
    set target [.target-define.top.target.entry get]
    if {![string length $target]} {
        return
    }
    foreach n [array names profile *,host] {
        if {![string compare $n ${target},host]} {
            destroy .target-define
            protocol-setup $n
            return
        }
    }
    foreach n [array names profile Default,*] {
        set profile($target,[string range $n 8 end]) $profile($n)
    }
    incr profile(Default,windowNumber)
    
#    debug-window "Target er her $target"
    protocol-setup $target
    destroy .target-define
}

# Procedure fail-response {target}
#  target   current target
# Error handler (IrTcl failback) that takes care of serious protocol
# errors, connection lost, etc.
proc fail-response {target} {
    global debugMode

    set c [lindex [z39 failInfo] 0]
    set m [lindex [z39 failInfo] 1]
    if {$c == 4 || $c == 5} {
        set debugMode 1        
        apduDump
    }
    close-target
#    tkerror "$m ($c)"
    bgerror "$m ($c)"
}

# Procedure connect-response {target base}
#  target   current target
#  base     current database
# IrTcl connect response handler.
proc connect-response {target base} {
    dputs "connect-response"
    init-request $target $base
}

# Procedure open-target {target base}
#  target   target to be opened
#  base     database to be used
# Opens a new connection with $target/$base.
proc open-target {target base} {
    global profile hostid presentChunk currentDb

    z39 disconnect
    z39 comstack $profile($target,comstack)
    z39 protocol $profile($target,protocol)
    eval z39 idAuthentication $profile($target,authentication)
    z39 maximumRecordSize $profile($target,maximumRecordSize)
    z39 preferredMessageSize $profile($target,preferredMessageSize)
    dputs "maximumRecordSize=[z39 maximumRecordSize]"
    dputs "preferredMessageSize=[z39 preferredMessageSize]"
    show-status Connecting 1 0
    set x $profile($target,largeSetLowerBound)
    if {![string length $x]} {
        set x 2
    }
    z39 largeSetLowerBound $x
    
    set x $profile($target,smallSetUpperBound)
    if {![string length $x]} {
        set x 0
    }
    z39 smallSetUpperBound $x
    
    set x $profile($target,mediumSetPresentNumber)
    if {![string length $x]} {
        set x 0
    }
    z39 mediumSetPresentNumber $x

    set presentChunk $profile($target,presentChunk)
    if {![string length $presentChunk]} {
        set presentChunk 4
    }

    z39 failback [list fail-response $target]
    z39 callback [list connect-response $target $base]
    show-target $target $base
    update idletasks
    set err [catch {
        z39 connect $profile($target,host):$profile($target,port)
    } errorMessage]
    if {$err} {
        set hostid Default
#        tkerror $errorMessage
        bgerror $errorMessage
        show-status "Not connected" 0 {}
        show-target {} {}
        return
    }
    set hostid $target
    set currentDb $base
    configure-disable-e .top.target.m 0
    configure-enable-e .top.target.m 1
    configure-enable-e .top.target.m 2
}

# Procedure close-target
# Shuts down the connection with current target.
proc close-target {} {
    global hostid cancelFlag setNo setNoLast currentDb

    set cancelFlag 0
    set setNo 0
    set setNoLast 0
    .bot.a.set configure -text ""
    set hostid Default
    set currentDb Default
    z39 disconnect
    show-target {} {}
    show-status {Not connected} 0 0
#    .top.options.m.query.slist entryconfigure 2 -state disabled
    configure-enable-e .top.options.m.query.slist 2
    init-title-lines
    show-message {}
    configure-disable-e .top.target.m 1
    configure-disable-e .top.target.m 2
    .top.rset.m delete 1 last
    .top.rset.m add separator
    configure-enable-e .top.target.m 0
}

# Procedure load-set-action
# Loads records from a file. The filename is read from the entry
# .load-set.filename.entry (see function load-set)
proc load-set-action {} {
    global setNoLast

    incr setNoLast
    ir-set z39.$setNoLast z39

    set fname [.load-set.top.filename.entry get]
    destroy .load-set
    if {$fname != ""} {
        show-status Loading 1 {}
        update
        z39.$setNoLast loadFile $fname

        set no [z39.$setNoLast numberOfRecordsReturned]
        add-title-lines $setNoLast $no 1
    }
    set l [format "%-4d %7d" $setNoLast $no]
    .top.rset.m add command -label $l \
            -command [list add-title-lines $setNoLast 10000 1]
    show-status Ready 0 {}
}

# Procedure load-set
# Dialog that asks for a filename with records to be loaded
# into a result set.
proc load-set {} {
    set w .load-set
    toplevel $w
    set oldFocus [focus]
    place-force $w .
    top-down-window $w

#    frame $w.top.filename
    frame $w.top.left
    frame $w.top.right
#    pack $w.top.filename -side top -anchor e -pady 2
    pack $w.top.left $w.top.right -side left -anchor e -pady 2
      
    entry-fields $w.top {left} {{Filename:}} {load-set-action} {destroy .load-set}
    button $w.top.right.but -text "Browse ..." \
        -command "fileDialog $w $w.top.left.entry open"
    pack $w.top.right.but -side right
    
    top-down-ok-cancel $w {load-set-action} 1
#    focus $oldFocus
    focus $w
}

proc fileDialog {w ent operation} {
    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
    	{"Text files"		{.txt}	            }
    	{"Text files"		{}		        TEXT}
    	{"Tcl Scripts"		{.tcl}		    TEXT}
    	{"All files"		*}
    }
    if {$operation == "open"} {
	set file [tk_getOpenFile -filetypes $types -parent $w]
    } else {
	set file [tk_getSaveFile -filetypes $types -parent $w \
	    -initialfile Untitled -defaultextension .txt]
    }
    if [string compare $file ""] {
	$ent delete 0 end
	$ent insert 0 $file
	$ent xview end
    }
}


# Procedure init-request
# Sends an initialize request to the target. This procedure is called
# when a connect has been established.
proc init-request {target base} {
    global cancelFlag

    if {$cancelFlag} {
        close-target
        return
    }
    z39 callback [list init-response $target $base]
    show-status Initializing 1 {}
    set err [catch {z39 init} errorMessage]
    if {$err} {
#        tkerror $errorMessage
        bgerror $errorMessage
        show-status Ready 0 {}
    }
}

# Procedure init-response
# Handles and incoming init-response. The service buttons
# are enabled. The global $scanEnable indicates whether the target
# supports scan.
proc init-response {target base} {
    global cancelFlag profile scanEnable settingsChanged

    dputs {init-response}
    apduDump
    if {$cancelFlag} {
        close-target
        return
    }
    if {![z39 initResult]} {
        set u [z39 userInformationField]
        close-target
#        tkerror "Connection rejected by target: $u"
        bgerror "Connection rejected by target: $u"
    } else {
        z39 failback [list explain-crash $target $base]
        explain-check $target [list ready-response $base] $base
    }
}

# Procedure explain-crash
# Handles target that dies during explain.
proc explain-crash {target base} {
    global profile settingsChanged
    
    set profile($target,timeLastInit) [clock seconds]
    set settingsChanged 1

    show-message {}
    open-target $target $base
}

# Procedure ready-response
# Called after a target has been initialized and, possibly, explained
proc ready-response {base target} {
    global profile settingsChanged scanEnable queryAuto
    
    z39 failback [list fail-response $target]
    if {[string length $base]} {
    set profile($target,timeLastInit) [clock seconds]
    set settingsChanged 1

    z39 databaseNames $base
    cascade-dblist $target $base
    show-target $target $base
    }
    if {[lsearch [z39 options] scan] >= 0} {
        set scanEnable 1
    } else {
        set scanEnable 0
    }
    .data.record delete 1.0 end
    set desc [string trim $profile($target,description)]
    if {[string length $desc]} {
        .data.record insert end "$desc\n\n"
    } else {
        .data.record insert end "$target\n\n"
    }
    set data [string trim $profile($target,welcomeMessage)]
    if {[string length $data]} {
        .data.record insert end "Welcome Message:\n$data\n\n"
    }
    set data [string trim $profile($target,recentNews)]
    if {[string length $data]} {
        .data.record insert end "News:\n$data\n"
    }
    ready-response-actions $target $base
    show-message {}
    show-status Ready 0 1
}

#proc ready-response-actions {target base}
#This procedure take care of all the actions that should start if connect is succesfull.
proc ready-response-actions {target base} {
    global profile queryAuto attributeTypeSelected queryTypes
    set autoNr [lsearch $queryTypes Auto]
    configureOptionsSyntax $target $base
#    if {[info exists profile($target,AttributeDetails,$base,\
#        $attributeTypeSelected)] && $queryAuto == 1} 
    if {[info exists profile($target,AttributeDetails,$base,$attributeTypeSelected)]} {
        changeQueryButtons $target $base
        change-queryInfo $target $base
        .top.options.m.query.slist entryconfigure $autoNr -state normal
        .top.options.m.query.clist entryconfigure $autoNr -state normal
        if {$queryAuto == 1} {
            query-select $autoNr
        } else {
            query-select 0
        }
    } else {
        query-select 0
#        .top.options.m.query.slist entryconfigure $autoNr -state disabled
        configure-disable-e .top.options.m.query.slist $autoNr
#        if {![info exists profile($target,AttributeDetails,$base,$attributeTypeSelected)]} 
#            .top.options.m.query.clist entryconfigure $autoNr -state disabled
            configure-disable-e .top.options.m.query.clist $autoNr
#        
    }
    if {[info exists attributeTypeSelected]} {
        global attribute[set attributeTypeSelected]
        set attribute$attributeTypeSelected 1
    } else {
        global attributeBib1
        set attributeBib1 1
    }
}

# Procedure search-request
#  bflag     flag to indicate if this procedure calls itself
# Performs a search. If $busy is 1, the search-request is performed
# at a later time (when another response arrives). This procedure
# sets many search-related Z39-settings. The global $setNo is set
# to the result set number (z39.$setNo).
proc search-request {bflag} {
    global setNo setNoLast profile hostid busy cancelFlag delayRequest \
        recordSyntax elementSetNames

    set target $hostid
    
    if {![string length [z39 connect]]} {
        return
    }
    dputs "search-request"
    show-message {}
    if {!$bflag && $busy} {
        dputs "busy: search-request ignored"
        return
    }
    if {$cancelFlag} {
        dputs "cancelFlag"
        show-status Searching 1 0
        set delayRequest {search-request 1}
        return
    }
    set delayRequest {} 

    set query [index-query]
    debug-window "Query er her: \"${query}\""
    if {![string length $query]} {
        return
    }
    incr setNoLast
    set setNo $setNoLast
    ir-set z39.$setNo z39
    
    if {$profile($target,namedResultSets) == 1} {
        z39.$setNo setName $setNo
        dputs "setName=${setNo}"
    } else {
        z39.$setNo setName default
        dputs "setName=default"
    }
    if {$profile($target,queryRPN) == 1} {
        z39.$setNo queryType rpn
    } elseif {$profile($target,queryCCL) == 1} {
        z39.$setNo queryType ccl
    }
    dputs Setting
    dputs $recordSyntax
    if {![string compare $recordSyntax None]} {
        z39.$setNo preferredRecordSyntax {}
    } else {
        z39.$setNo preferredRecordSyntax $recordSyntax
    }
    if {![string compare $elementSetNames None]} {
        z39.$setNo elementSetNames {}
        z39.$setNo smallSetElementSetNames {}
        z39.$setNo mediumSetElementSetNames {}
    } else {
        z39.$setNo elementSetNames $elementSetNames
        z39.$setNo smallSetElementSetNames $elementSetNames
        z39.$setNo mediumSetElementSetNames $elementSetNames
    }
    z39 callback {search-response}
    z39.${setNo} search $query
    show-status Searching 1 0
}

# Procedure scan-copy {y entry}
#  y       y-position of mouse pointer
#  entry   a search entry in the top
# Copies the term in the list nearest $y to the query entry specified
# by $entry
proc scan-copy {y entry} {
    set w .scan-window
    set no [$w.top.list nearest $y]
    dputs "no=$no"
    .lines.$entry.e delete 0 end
    .lines.$entry.e insert 0 [string range [$w.top.list get $no] 8 end]
}

# Procedure scan-request
# Performs a scan on term "0" with the current attributes in entry
# specified by the global $curIndexEntry.
proc scan-request {} {
    set w .scan-window

    global profile hostid scanView scanTerm curIndexEntry queryButtonsFind \
        queryInfoFind cancelFlag delayRequest

    dputs "scan-request"
    if {$cancelFlag} {
        dputs "cancelFlag"
        show-status Scanning 1 0
        set delayRequest scan-request
        return
    }
    set delayRequest {} 
    set target $hostid
    set scanView 0
    set scanTerm {}
    set b [lindex $queryButtonsFind $curIndexEntry]
    set attr {}
    foreach a [lrange [lindex $queryInfoFind [lindex $b 1]] 1 end] {
        set attr "@attr $a $attr"
    }
    set title [lindex [lindex $queryInfoFind [lindex $b 1]] 0]
    ir-scan z39.scan z39

    if {![winfo exists $w]} {
        toplevelG $w
        
        wm minsize $w 0 0

        top-down-window $w

        entry $w.top.entry -relief sunken 
        pack $w.top.entry -fill x -padx 4 -pady 2
        bind $w.top.entry <KeyRelease> [list scan-term-h $attr]
        listbox $w.top.list -yscrollcommand [list $w.top.scroll set] \
            -font fixed -background white
        scrollbar $w.top.scroll -orient vertical -border 1
        pack $w.top.list -side left -fill both -expand yes
        pack $w.top.scroll -side right -fill y
        $w.top.scroll config -command [list $w.top.list yview]
        
        bottom-buttons $w [list {Close} [list destroy $w] \
            {Up} [list scan-up $attr] {Down} [list scan-down $attr]] 0
        bind $w.top.list <Up> [list scan-up $attr]
        bind $w.top.list <Down> [list scan-down $attr]
        focus $w.top.entry
    }
    bind $w.top.list <Double-Button-1> [list scan-copy %y $curIndexEntry]
    wm title $w "Scan $title"
        
    z39 callback [list scan-response $attr 0 35]
    z39.scan numberOfTermsRequested 5
    z39.scan preferredPositionInResponse 1
    z39.scan scan "${attr} 0"
    
    show-status Scanning 1 0
}

# Procedure scan-term-h {attr} 
# attr    attribute specification
# This procedure is called whenever a key is released in the entry in the
# scan window (.scan-window). A scan is then initiated with the new contents
# of the entry as the starting term.
proc scan-term-h {attr} {
    global busy scanTerm

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
    dputs "${attr} \{${scanTerm}\}"
    if {![string length $scanTerm]} {
        z39.scan scan "${attr} 0"
    } else {
        z39.scan scan "${attr} \{${scanTerm}\}"
    }
    show-status Scanning 1 0
}

# Procedure scan-response {attr start toget}
#  attr   attribute specification
#  start  position of first term in the response
#  toget  number of total terms to get
# This procedure handles all scan-responses. $start specifies the list
# entry number of the first incoming term. The $toget indicates the total
# number of terms to be retrieved from the target. The $toget may be
# negative in which case, scan is performed 'backwards' (- $toget is
# the total number of terms in this case). This procedure usually calls
# itself several times in order to get small scan-term-list chunks.
proc scan-response {attr start toget} {
    global cancelFlag delayRequest scanTerm scanView

    set w .scan-window
    dputs "In scan-response"
    apduDump
    set m [z39.scan numberOfEntriesReturned]
    dputs $m
    dputs attr=$attr
    dputs start=$start
    dputs toget=$toget

    if {![winfo exists .scan-window]} {
        if {$cancelFlag} {
            set cancelFlag 0
            dputs "Handling cancel"
            if {$delayRequest != ""} {
                eval $delayRequest
            }
            return
        }
        show-status Ready 0 1
        return
    }
    set nScanTerm [$w.top.entry get]
    if {$nScanTerm != $scanTerm} {
        z39 callback [list scan-response $attr 0 35]
        z39.scan numberOfTermsRequested 5
        z39.scan preferredPositionInResponse 1
        set scanTerm $nScanTerm
        dputs "${attr} \{${scanTerm}\}"
        if {![string length $scanTerm]} {
            z39.scan scan "${attr} 0"
        } else {
            z39.scan scan "${attr} \{${scanTerm}\}"
        }
        show-status Scanning 1 0
        return
    }
    set status [z39.scan scanStatus]
    if {$status == 6} {
#        tkerror "Scan fail"
        bgerror "Scan fail"
        show-status Ready 0 1
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
        dputs "Handling cancel"
        set cancelFlag 0
        if {$delayRequest != ""} {
            eval $delayRequest
        }
        return
    }
    set delayRequest {}
    if {$toget > 0 && $m > 1 && $m < $toget} {
        set ntoget [expr $toget - $m + 1]
        dputs ntoget=$ntoget
        z39 callback [list scan-response $attr [expr $start + $m - 1] $ntoget]
        set q $term
        dputs "down continue: $q"
        if {$ntoget > 10} {
            z39.scan numberOfTermsRequested 10
        } else {
            z39.scan numberOfTermsRequested $ntoget
        }
        z39.scan preferredPositionInResponse 1
        dputs "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    if {$toget < 0 && $m > 1 && $m < [expr - $toget]} {
        set ntoget [expr - $toget - $m]
        dputs ntoget=$ntoget
        z39 callback [list scan-response $attr 0 -$ntoget]
        set q [string range [$w.top.list get 0] 8 end]
        dputs "up continue: $q"
        if {$ntoget > 10} {
            z39.scan numberOfTermsRequested 10
            z39.scan preferredPositionInResponse 11
        } else {
            z39.scan numberOfTermsRequested $ntoget
            z39.scan preferredPositionInResponse [incr ntoget]
        }
        dputs "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    show-status Ready 0 1
}

# Procedure scan-down {attr}
#  attr   attribute specification
# This procedure is called when the user hits the Down button the scan
# window. A new scan is initiated with a positive $toget passed to the
# scan-response handler.
proc scan-down {attr} {
    global scanView cancelFlag delayRequest

    dputs {scan-down}
    if {$cancelFlag} {
        dputs "cancelFlag"
        show-status {Scanning down} 1 0
        set delayRequest [list scan-down $attr]
        return
    }
    set delayRequest {} 

    set w .scan-window
    set scanView [expr $scanView + 5]
    set s [$w.top.list size]
    if {$scanView > $s} {
        z39 callback [list scan-response $attr [expr $s - 1] 25]
        set q [string range [$w.top.list get [expr $s - 1]] 8 end]
        dputs "down: $q"
        z39.scan numberOfTermsRequested 10
        z39.scan preferredPositionInResponse 1
        show-status Scanning 1 0
        dputs "${attr} \{$q\}"
        z39.scan scan "${attr} \{$q\}"
        return
    }
    $w.top.list yview $scanView
}

# Procedure scan-up {attr}
#  attr   attribute specification
# This procedure is called when the user hits the Up button the scan
# window. A new scan is initiated with a negative $toget passed to the
# scan-response handler.
proc scan-up {attr} {
    global scanView cancelFlag delayRequest

    dputs {scan-up}
    if {$cancelFlag} {
        dputs "cancelFlag"
        show-status Scanning 1 0
        set delayRequest [list scan-up $attr]
        return
    }
    set delayRequest {} 

    set w .scan-window
    set scanView [expr $scanView - 5]
    if {$scanView < 0} {
        z39 callback [list scan-response $attr 0 -25]
        set q [string range [$w.top.list get 0] 8 end]
        dputs "up: $q"
        z39.scan numberOfTermsRequested 10
        z39.scan preferredPositionInResponse 11
        show-status Scanning 1 0
        z39.scan scan "${attr} \{$q\}"
        return
    }
    $w.top.list yview $scanView
}

# Procedure search-response
# This procedure handles search-responses. If the search is successful
# this procedure will try to retrieve a total of 20 records from the target;
# however not more than $presentChunk records at a time. This procedure
# affects the following globals:
#   $setOffset        current record position offset
#   $setMax           total number of records to be retrieved
proc search-response {} {
    global setNo setOffset setMax cancelFlag busy delayRequest presentChunk

    apduDump
    dputs "In search-response"
    if {$cancelFlag} {
        dputs "Handling cancel"
        set cancelFlag 0
        if {$delayRequest != ""} {
            eval $delayRequest
        }
        return
    }
    set setOffset 0
    set delayRequest {}
    init-title-lines
    set setMax [z39.$setNo resultCount]
    show-status Ready 0 1
    set status [z39.$setNo responseStatus]
    if {![string compare [lindex $status 0] NSD]} {
        z39.$setNo nextResultSetPosition 0
        set code [lindex $status 1]
        set msg [lindex $status 2]
        set addinfo [lindex $status 3]
#        tkerror "NSD$code: $msg: $addinfo"
        bgerror "NSD$code: $msg: $addinfo"
        return
    }
    show-message "${setMax} hits"
    if {$setMax == 0} {
        return
    }
    set setOffset 1
    show-status Ready 0 1
    set l [format "%-4d %7d" $setNo $setMax]
    .top.rset.m add command -label $l -command [list recall-set $setNo]
    if {$setMax > 20} {
        set setMax 20
    }
    set no [z39.$setNo numberOfRecordsReturned]
    dputs "Returned $no records, setOffset $setOffset"
    add-title-lines $setNo $no $setOffset
    set setOffset [expr $setOffset + $no]

    set toGet [expr $setMax - $setOffset + 1]
    if {$toGet > 0} {
        if {$setOffset == 1} {
            set toGet 1
        } elseif {$toGet > $presentChunk} {
            set toGet $presentChunk
        }
        z39 callback {present-response}
        z39.$setNo present $setOffset $toGet
        show-status Retrieving 1 0
    }
}

# Procedure present-more {number}
#  number      number of records to be retrieved
# This procedure starts a present-request. The $number variable indicates
# the total number of records to be retrieved. The global $presentChunk
# specifies the number of records to be retrieved at a time. If $number
# is the empty string all remaining records in the result set are 
# retrieved.
proc present-more {number} {
    global setNo setOffset setMax busy cancelFlag delayRequest presentChunk

    dputs "present-more"
    if {$cancelFlag} {
        show-status Retrieving 1 0
        set delayRequest "present-more $number"
        return
    }
    set delayRequest {}

    if {$setNo == 0} {
        dputs "setNo=$setNo"
    return
    }
    set setOffset [z39.$setNo nextResultSetPosition]
    dputs "setOffest=${setOffset}"
    dputs "setNo=${setNo}"
    set max [z39.$setNo resultCount]
    if {$max < $setOffset} {
        dputs "max=$max"
        dputs "setOffset=$setOffset"
        show-status Ready 0 1
        return
    }
    if {![string length $number]} {
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
    if {$toGet > $presentChunk} {
        set toGet $presentChunk
    } 
    z39.$setNo present $setOffset $toGet
    show-status Retrieving 1 0
}

# Procedure init-title-lines 
# Utility that cleans the main record window.
proc init-title-lines {} {
    .data.record delete 1.0 end
}

# Procedure recall-set {setno}
#  setno    Set number to recall
proc recall-set {setno} {
    add-title-lines $setno 10000 1
}

# Procedure add-title-lines {setno no offset}
#  setno    Set number
#  no       Number of records
#  offset   Starting offset
# This procedure displays the records $offset .. $offset+$no-1 in result
# set $setno in the main record window by using the display format in the
# global $displayFormat
proc add-title-lines {setno no offset} {
    global displayFormats displayFormat setNo busy

    dputs "add-title-lines offset=${offset} no=${no}"
    if {$setno != -1} {
        set setNo $setno
    } else {
        set setno $setNo
    }
    if {$offset == 1} {
        .bot.a.set configure -text $setno
        .data.record delete 1.0 end
    }
    set ffunc [lindex $displayFormats $displayFormat]
    dputs "ffunc=$ffunc"
    set ffunc "display-$ffunc"
    for {set i 0} {$i < $no} {incr i} {
        set o [expr $i + $offset]
        set type [z39.$setno type $o]
        if {![string length $type]} {
            dputs "no more at $o"
            break
        }
        .data.record tag bind r$o <Any-Enter> {}
        .data.record tag bind r$o <Any-Leave> {}
        set insert0 [.data.record index insert]
        $ffunc $setno $o .data.record 1
        .data.record tag add r$o $insert0 insert
        .data.record tag bind r$o <1> [list popup-marc $setno $o 0 0]
        update idletasks
    }
    if {!$busy} {
        show-status Ready 0 1
    }
}

# Procedure present-response
# Present-response handler. The incoming records are displayed and a new
# present request is performed until all records ($setMax) is returned
# from the target.
proc present-response {} {
    global setNo setOffset setMax cancelFlag delayRequest presentChunk

    dputs "In present-response"
    apduDump
    set no [z39.$setNo numberOfRecordsReturned]
    dputs "Returned $no records, setOffset $setOffset"
    add-title-lines $setNo $no $setOffset
    set setOffset [expr $setOffset + $no]
    if {$cancelFlag} {
        dputs "Handling cancel"
        set cancelFlag 0
        if {$delayRequest != ""} {
            eval $delayRequest
        }
        return
    }
    set status [z39.$setNo responseStatus]
    if {![string compare [lindex $status 0] NSD]} {
        show-status Ready 0 1
        set code [lindex $status 1]
        set msg [lindex $status 2]
        set addinfo [lindex $status 3]
#        tkerror "NSD$code: $msg: $addinfo"
        bgerror "NSD$code: $msg: $addinfo"
        return
    }
    if {$no > 0 && $setOffset <= $setMax} {
        dputs "present-request from ${setOffset}"
        set toGet [expr $setMax - $setOffset + 1]
        if {$toGet > $presentChunk} {
            set toGet $presentChunk
        }
        z39.$setNo present $setOffset $toGet
    } else {
        show-status Ready 0 1
    }
}

# Procedure left-cursor {w}
#  w    entry widget
# Tries to move the cursor left in entry window $w
proc left-cursor {w} {
    set i [$w index insert]
    if {$i > 0} {
        incr i -1
        $w icursor $i
    }
    dputs left
}

# Procedure right-cursor {w}
#  w    entry widget
# Tries to move the cursor right in entry window $w
proc right-cursor {w} {
    set i [$w index insert]
    incr i
    dputs right
    $w icursor $i
}

# Procedure bind-fields {list returnAction escapeAction}
#  list          list of entry widgets
#  returnAction  return script
#  escapeAction  escape script
# Each widget in list are assigned bindings for <Tab>, <Left>, <Right>,
# <Return> and <Escape>.
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
    bind [lindex $list $i] <Tab>  [list focus [lindex $list 0]]
    bind [lindex $list $i] <Left> [list left-cursor [lindex $list $i]]
    bind [lindex $list $i] <Right> [list right-cursor [lindex $list $i]]
    focus [lindex $list 0]
}

# Procedure entry-fields {parent list tlist returnAction escapeAction}
#  list          list of frame widgets
#  tlist         list of text to be used as lead of each entry
#  returnAction  return script
#  escapeAction  escape script
# Makes label and entry widgets in each widget in $list.
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

# Procedure define-target-dialog
# Dialog that asks for new target to be defined.
proc define-target-dialog {} {
    set w .target-define

    toplevel $w
    place-force $w .
    top-down-window $w
    frame $w.top.target
    pack $w.top.target -side top -anchor e -pady 2 
    entry-fields $w.top {target} {{Target:}} \
        {define-target-action} {destroy .target-define}
    top-down-ok-cancel $w {define-target-action} 1
}

# Procedure place-force {window parent}
#  window      new top level widget
#  parent      parent widget used as base
# Sets geometry of $window relative to $parent window.
proc place-force {window parent} {
    set g [wm geometry $parent]
    set p1 [string first + $g]
    set p2 [string last + $g]
    set x [expr 40+[string range $g [expr {$p1 + 1}] [expr {$p2 -1}]]]
    set y [expr 60+[string range $g [expr {$p2 + 1}] end]]
    wm geometry $window +${x}+${y}
}

# Procedure add-database-action {target w}
#  target      target to be defined
#  w           top level widget for the target definition
# Adds the contents of .database-select.top.database.entry to list of
# databases.
proc add-database-action {target w} {
    global profile

    $w.top.databases.list insert end [.database-select.top.database.entry get]
    destroy .database-select
}

# Procedure add-database {target wp}
#  target      target to be defined
#  wp          top level widget for the target definition
# Makes a dialog in which the user enters new database
proc add-database {target wp} {
    global profile

    set w .database-select
    toplevel $w
    set oldFocus [focus]
    place-force $w $wp
    top-down-window $w
    frame $w.top.database
    pack $w.top.database -side top -anchor e -pady 2
    entry-fields $w.top {database} {{Database to add:}} \
        [list add-database-action $target $wp] {destroy .database-select}

    top-down-ok-cancel $w [list add-database-action $target $wp] 1
    focus $oldFocus
}


# Procedure delete-database {target w}
#  target     target to be defined
#  w          top level widget for the target definition
# Asks the user if he/she really wishes to delete a database and removes
# the database from the database-list if requested.
proc delete-database {target w} {
    global profile

    set l {}
    foreach i [$w.top.databases.list curselection] {
        set b [$w.top.databases.list get $i]
        set l "$l $b"
    }
    set a [alert "Are you sure you want to remove the database(s)${l}?"]
    if {$a} {
        foreach i [lsort -decreasing \
                [$w.top.databases.list curselection]] {
            $w.top.databases.list delete $i
        }
    }
}

# Procedure advanced-setup {target b}
#  target     target to be defined
#  b          window number of target top level
# Makes a dialog in which the user may modify/view advanced settings
# of a target definition (profile).
proc advanced-setup {target b} {
    global profile profileS

    set w .advanced-setup-$b
    toplevelG $w
    wm title $w "Advanced setup $target"
    top-down-window $w
    if {![string length $target]} {
        set target Default
    }
    dputs target
    
    frame $w.top.largeSetLowerBound
    frame $w.top.smallSetUpperBound
    frame $w.top.mediumSetPresentNumber
    frame $w.top.presentChunk
    frame $w.top.maximumRecordSize
    frame $w.top.preferredMessageSize

    pack $w.top.largeSetLowerBound $w.top.smallSetUpperBound \
        $w.top.mediumSetPresentNumber $w.top.presentChunk \
        $w.top.maximumRecordSize $w.top.preferredMessageSize \
        -side top -anchor e -pady 2
    
    entry-fields $w.top {largeSetLowerBound smallSetUpperBound \
        mediumSetPresentNumber presentChunk maximumRecordSize \
        preferredMessageSize} \
        {{Large Set Lower Bound:} {Small Set Upper Bound:} \
        {Medium Set Present Number:} {Present Chunk:} \
        {Maximum Record Size:} {Preferred Message Size:}} \
        [list advanced-setup-action $target $b] [list destroy $w]

    $w.top.largeSetLowerBound.entry configure -textvariable \
    profileS($target,largeSetLowerBound)
    $w.top.smallSetUpperBound.entry configure -textvariable \
    profileS($target,smallSetUpperBound)
    $w.top.mediumSetPresentNumber.entry configure -textvariable \
    profileS($target,mediumSetPresentNumber)
    $w.top.presentChunk.entry configure -textvariable \
    profileS($target,presentChunk)
    $w.top.maximumRecordSize.entry configure -textvariable \
    profileS($target,maximumRecordSize)
    $w.top.preferredMessageSize.entry configure -textvariable \
    profileS($target,preferredMessageSize)
    
    bottom-buttons $w [list {Ok} [list advanced-setup-action $target $b] \
            {Cancel} [list destroy $w]] 0   
}

# Procedure advanced-setup-action {target b}
#  target     target to be defined
#  b          window number of target top level
# This procedure is called when the user hits Ok in the advanced target
# setup dialog. The temporary result is stored in the $profileS - array.
proc advanced-setup-action {target b} {
    set w .advanced-setup-$b
    global profileS
    
    set profileS($target,LSLB) [$w.top.largeSetLowerBound.entry get]
    set profileS($target,SSUB) [$w.top.smallSetUpperBound.entry get]
    set profileS($target,MSPN) [$w.top.mediumSetPresentNumber.entry get]
    set profileS($target,presentChunk) [$w.top.presentChunk.entry get]
    set profileS($target,MRS) [$w.top.maximumRecordSize.entry get]
    set profileS($target,PMS) [$w.top.preferredMessageSize.entry get]

    dputs "advanced-setup-action"
    destroy $w
}

# Procedure database-select-action
# Called when the user commits a database select change. See procedure
# database-select.
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

# Procedure database-select
# Makes a dialog in which the user may select a database
proc database-select {} {
    set w .database-select
    global profile hostid

    toplevel $w
    set oldFocus [focus]
    place-force $w .
    top-down-window $w

    frame $w.top.databases -relief ridge -border 2
    pack $w.top.databases -side left -pady 6 -padx 6 -expand yes -fill x

    label $w.top.databases.label -text "List"
    listbox $w.top.databases.list -width 20 -height 6 \
        -yscrollcommand "$w.top.databases.scroll set"
    scrollbar $w.top.databases.scroll -orient vertical -border 1
    pack $w.top.databases.label -side top -fill x -padx 2 -pady 2
    pack $w.top.databases.list -side left -fill both -expand yes -padx 2 -pady 2
    pack $w.top.databases.scroll -side right -fill y -padx 2 -pady 2
    $w.top.databases.scroll config -command "$w.top.databases.list yview"

    foreach b $profile($hostid,databases) {
        $w.top.databases.list insert end $b
    }
    top-down-ok-cancel $w {database-select-action} 1
    focus $oldFocus
}

# Procedure cascase-dblist-select
proc cascade-dblist-select {target db} {
    show-target $target $db
    z39 databaseNames $db
    ready-response-actions $target $db
}

# Procedure cascade-dblist 
# Makes the Service/database list with proper databases for the target
proc cascade-dblist {target base} {
    global profile

    set w .top.service.m.dblist
#    $w delete 0 200
    $w delete 0 end
    if {[info exists profile($target,databases)]} {
        foreach db $profile($target,databases) {
            $w add command -label $db \
            -command [list cascade-dblist-select $target $db]
        }
    }
}

# Procedure cascade-target-list
# Makes all target/databases available in the Target|Connect
# menu as well as all targets in the Target|Setup menu.
# This procedure is called whenever target definitions occur.
proc cascade-target-list {} {
    global profile
    
    foreach sub [winfo children .top.target.m.clist] {
        destroy $sub
    }
    .top.target.m.clist delete 0 last
    foreach nn [lsort [array names profile *,host]] {
        if {[string length $profile($nn)]} {
            set ll [expr {[string length $nn] - 6}]
            set n [string range $nn 0 $ll]
            
            set nl $profile($n,windowNumber)
            if {[info exists profile($n,databases)]} {
                set ndb [llength $profile($n,databases)]
            } else {
                set ndb 0
            }
            if {$ndb > 1} {
                .top.target.m.clist add cascade -label $n \
                    -menu .top.target.m.clist.$nl
                irmenu .top.target.m.clist.$nl
                foreach b $profile($n,databases) {
                    .top.target.m.clist.$nl add command -label $b \
                    -command [list reopen-target $n $b]
                }
            } elseif {$ndb == 1} {
                .top.target.m.clist add command -label $n -command \
                    [list reopen-target $n [lindex $profile($n,databases) 0]]
            } else {
                .top.target.m.clist add command -label $n -command \
                    [list reopen-target $n {}]
            }
        }
    }
    .top.target.m.slist delete 0 last
    foreach nn [lsort [array names profile *,host]] {
        set ll [expr {[string length $nn] - 6}]
        set n [string range $nn 0 $ll]
        .top.target.m.slist add command -label $n -command [list protocol-setup $n]
    }
}

# Procedure query-select {i}
#  i       Query type number (integer)
# This procedure is called when the user selects a Query type. The current
# query type information given by the globals $queryButtonsFind and
# $queryInfoFind are changed by this operation.
proc query-select {i} {
    global queryButtonsFind queryInfoFind  queryTypes queryAuto queryAutoOld \
        hostid currentDb profile attributeTypeSelected
    global queryButtons$attributeTypeSelected queryInfo$attributeTypeSelected
    foreach n $queryTypes {
        global query$n
        set query$n 0
    }
    set query[lindex $queryTypes $i] 1
    if {$queryAutoOld && !$queryAuto} {
        set queryAutoOld $queryAuto
    }
    if {!$queryAutoOld && $queryAuto && ![info exists profile($hostid,AttributeDetails,$currentDb,[lindex $queryTypes $i])]} {
        set queryAutoOld $queryAuto
    }
    set queryInfoFind [lindex [set queryInfo$attributeTypeSelected] $i]
    set queryButtonsFind [lindex [set queryButtons$attributeTypeSelected] $i]
    index-lines .lines 1 $queryButtonsFind $queryInfoFind activate-index
}

# Procedure query-new-action 
# Commits a new query type definition by extending the globals
# $queryTypes, $queryButtons and $queryInfo.
proc query-new-action {} {
    global queryTypes settingsChanged attributeTypeSelected
    global queryButtons$attributeTypeSelected queryInfo$attributeTypeSelected
    set settingsChanged 1
    lappend queryTypes [.query-new.top.index.entry get]
    lappend queryButtons$attributeTypeSelected {}
    lappend queryInfo$attributeTypeSelected {}

    destroy .query-new
    cascade-query-list
}

# Procedure query-new
# Makes a dialog in which the user is requested to enter the name of a
# new query type.
proc query-new {} {
    set w .query-new

    toplevel $w
    set oldFocus [focus]
    place-force $w .
    top-down-window $w
    frame $w.top.index
    pack $w.top.index -side top -anchor e -pady 2 
    entry-fields $w.top index {{Query Name:}} \
            query-new-action {destroy .query-new}
    top-down-ok-cancel $w query-new-action 1
    focus $oldFocus
}

# Procedure query-delete-action {queryNo}
#  queryNo     query type number (integer)
# Procedure that deletes the query type specified by $queryNo.
proc query-delete-action {queryNo} {
    global queryTypes settingsChanged attributeTypeSelected
    global queryInfo$attributeTypeSelected queryButtons$attributeTypeSelected

    set settingsChanged 1

    set queryTypes [lreplace $queryTypes $queryNo $queryNo]
    set queryButtons$attributeTypeSelected [lreplace [set queryButtons$attributeTypeSelected] \
        $queryNo $queryNo]
    set queryInfo$attributeTypeSelected [lreplace [set queryInfo$attributeTypeSelected] \
        $queryNo $queryNo]
    destroy .query-delete
    cascade-query-list
}

# Procedure query-delete {queryNo}
#  queryNo     query type number (integer)
# Asks if the user really want to delete a given query type; calls
# query-delete-action if 'yes'.
proc query-delete {queryNo} {
    global queryTypes

    set w .query-delete

    toplevel $w
    place-force $w .
    top-down-window $w
    set n [lindex $queryTypes $queryNo]

    label $w.top.warning -bitmap warning
    message $w.top.quest -text "Are you sure you want to delete the \
        query type $n ?"  -aspect 300
    pack $w.top.warning $w.top.quest -side left -expand yes -padx 10 -pady 5
    bottom-buttons $w [list {Ok} [list query-delete-action $queryNo] \
        {Cancel} [list destroy $w]] 1
}

# Procedure cascade-query-list
# Updates the entries below Options|Query to list all query types.
proc cascade-query-list {} {
    global queryTypes hostid attributeTypes queryAuto
    set w .top.options.m.query
    set i 0
    $w.clist delete 0 last
    foreach n $queryTypes {
        $w.clist add check -label $n -variable query$n -command [list query-select $i]
        global query$n
        incr i
    }
    set i 0
    $w.slist delete 0 last
    foreach n $queryTypes {
        if {$n == "Auto"} {
            if {$hostid == "Default"} {
                $w.slist add command -label $n -state disabled \
                    -command [list query-setup $i]
            } else {
                $w.slist add command -label $n -command [list query-setup $i]
            }
        } else {
            $w.slist add command -label $n -command [list query-setup $i]
        }
        incr i
    }
    set i 0
    $w.tlist delete 0 last
    foreach n $attributeTypes {
        global attribute$n
        $w.tlist add check -label $n -variable attribute$n \
            -command [list attribute-select $i]
        incr i
    }
    set i 0
    $w.dlist delete 0 last
    foreach n $queryTypes {
        if {$n == "Auto"} {
            $w.dlist add command -label $n -state disabled \
                -command [list query-setup $i]
        } else {
            $w.dlist add command -label $n -command [list query-delete $i]
        }
        incr i
    }
}

# Procedure save-geometry
# This procedure saves the per-user related settings in ~/.clientrc.tcl.
# The geometry information stored in the global array $windowGeometry is
# saved. Also a few other user settings, such as current display format, are
# saved.
proc save-geometry {} {
    global windowGeometry hotTargets textWrap displayFormat popupMarcdf \
        recordSyntax elementSetNames hostid

    set windowGeometry(.) [wm geometry .]

    if {[catch {set f [open ~/.clientrc.tcl w]}]} {
        return
    } 
    if {$hostid != "Default"} {
        puts $f "set hostid [list $hostid]"
        set b [z39 databaseNames]
        puts $f "set hostbase [list $b]"
    }
    puts $f "set hotTargets [list $hotTargets]"
    puts $f "set textWrap $textWrap"
    puts $f "set displayFormat $displayFormat"
    puts $f "set popupMarcdf $popupMarcdf"
    puts $f "set recordSyntax $recordSyntax"
    puts $f "set elementSetNames $elementSetNames"
    foreach n [array names windowGeometry] {
        puts -nonewline $f "set [list windowGeometry($n)] "
        puts $f [list $windowGeometry($n)]
    }
    close $f
}

# Procedure save-settings
# This procedure saves the per-host related settings irtdb.tcl which
# is normally kept in the directory /usr/local/lib/irtcl.
# All query types and target defintion profiles are saved.
proc save-settings {} {
    global profile libdir settingsChanged queryTypes queryAuto \
        attributeTypes attributeTypeSelected
    if {[file writable [file join $libdir irtdb.tcl]]} {
        set f [open [file join $libdir irtdb.tcl] w]
    } else {
        set f [open "irtdb.tcl" w]
    }
    puts $f "# Setup file"
    foreach n [lsort [array names profile]] {
        puts $f "set [list profile($n)] [list $profile($n)]"
    }
    puts $f "set attributeTypes [list $attributeTypes]"
    puts $f "set attributeTypeSelected [list $attributeTypeSelected]"
    puts $f "set queryTypes [list $queryTypes]"
    foreach attrtype $attributeTypes {
        global queryButtons$attrtype queryInfo$attrtype
        catch {puts $f "set queryButtons$attrtype [list [set queryButtons$attrtype]]"}
        catch {puts $f "set queryInfo$attrtype [list [set queryInfo$attrtype]]"}
    }
    puts $f "set queryAuto [list $queryAuto]"
    close $f
    set settingsChanged 0
}

# Procedure alert {ask}
#  ask    prompt string
# Makes a grabbed dialog in which the user is requested to answer
# "Ok" or "Cancel". This procedure returns 1 if the user hits "Ok"; 0
# otherwise.
proc alert {ask} {
    set w .alert

    global alertAnswer font

    toplevel $w
    set oldFocus [focus]
    place-force $w .
    top-down-window $w

    label $w.top.warning -bitmap warning
    message $w.top.message -text $ask -aspect 300 -font $font(b,normal)
    pack $w.top.warning $w.top.message -side left -pady 5 -padx 10 -expand yes
  
    set alertAnswer 0
    top-down-ok-cancel $w {alert-action} 1
    focus $oldFocus
    return $alertAnswer
}

# Procedure alert-action
# Called when the user hits "Ok" in the .alert-window.
proc alert-action {} {
    global alertAnswer
    set alertAnswer 1
    destroy .alert
}

# Procedure exit-action
# This procedure is called if the user exists the application
proc exit-action {} {
    global settingsChanged

    if {$settingsChanged} {
        save-settings
    }
    save-geometry
    exit 0
}

# Procedure listbuttonaction {w name h user i}
#  w       menubutton widget
#  name    name information
#  h       handler to be invoked
#  user    user information to be passed to handler $h
#  i       index passed as second argument to handler $h
# Utility function to emulate a listbutton. Called when the user
# Modifies the listbutton. See procedure listbuttonx.
proc listbuttonaction {w name h user i} {
    $w configure -text [lindex $name 0]
    $h [lindex $name 1] $user $i
    if {[regexp {.lines.[ ]*([0-9]+).*} $w match j]} {
        global attributeTypeSelected queryTypes hostid currentDb profile
        global queryButtons$attributeTypeSelected
        set n -1
        foreach type $queryTypes {
            global query$type
            if {[set query$type] == 1} {
                set n [lsearch $queryTypes $type]
            }
        }
        if {$n == -1} {
            return
        }
        set list [lindex [set queryButtons$attributeTypeSelected] $n]
        set length [llength $list]
        if {$j < $length} {
#            set new [lreplace [lindex [set queryButtons$attributeTypeSelected] $n] $j $j [list I $i]]
            set new [lreplace $list $j $j [list I $i]]
        } else {
            set new [lappend $list [list I $i]]
        }
        set queryButtons$attributeTypeSelected \
            [lreplace [set queryButtons$attributeTypeSelected] $n $n $new]
        set profile($hostid,queryButtons,$currentDb) $new
        
    }
#    debug-window "[lindex $name 0], [lindex $name 1], i er her $i, winduesnavnet er $w, n er $n"
#    debug-window "new er $new"
}

# Procedure listbuttonx {button no names handle user}
#  button  menubutton widget
#  no      initial value index (integer)
#  names   list of name entries. The first entry in each name
#          entry is the actual name
#  handle  user function to be called when the listbutton changes
#          its value
#  user    user argument to the $handle function
# Makes an extended listbutton.
proc listbuttonx {button no names handle user} {
    set width 10
    foreach name $names {
        set buttonName [lindex $name 0]
        if {[string length $buttonName] > $width} {
            set width [string length $buttonName]
        }
    } 
    if {[winfo exists $button]} {
        $button configure -width $width -text [lindex [lindex $names $no] 0]
        ${button}.m delete 0 last
    } else {
        menubutton $button -text [lindex [lindex $names $no] 0] \
            -width $width -menu ${button}.m -relief raised -border 1
        irmenu ${button}.m
        ${button}.m configure -tearoff off
    }
    set i 0
    foreach name $names {
        ${button}.m add command -label [lindex $name 0] \
            -command [list listbuttonaction ${button} $name $handle $user $i]
        incr i
    }
}

# Procedure listbutton {button no names}
#  button  menubutton widget
#  no      initial value index (integer)
#  names   list of possible values.
# Makes a listbutton. The functionality is emulated by the use menubutton-
# and menu widgets.
proc listbutton {button no names} {
    menubutton $button -text [lindex $names $no] -width 10 -menu ${button}.m \
        -relief raised -border 1
    irmenu ${button}.m
    ${button}.m configure -tearoff off
    foreach name $names {
        ${button}.m add command -label $name \
                -command [list ${button} configure -text $name]
    }
}

# Procedure listbuttonv-action {button var names i}
#  button   menubutton widget
#  var      global variable to be affected
#  names    list of possible names and values
# This procedure is called when the user alters a menu created by the
# listbuttonv procedure. The global variable $var is updated.
proc listbuttonv-action {button var names i} {
    global $var

    set $var [lindex $names [expr {$i+1}]]
    $button configure -text [lindex $names $i]
}

# Procedure listbuttonv {button var names}
#  button   menubutton widget
#  var      global variable to be affected
#  names    List of name/value pairs, i.e. {n1 v1 n2 v2 ...}.
# This procedure emulates a listbutton by means of menu/menubutton widgets.
# The global variable $var is automatically updated and set to one of the
# values v1, v2, ...
proc listbuttonv {button var names} {
    global $var

    set n "-"
    set val [set $var]
    set l [llength $names]
    for {set i 1} {$i < $l} {incr i 2} {
        if {$val == [lindex $names $i]} {
            incr i -1
            set n [lindex $names $i]
            break
        }
    }
    if {[winfo exists $button]} {
        $button configure -text $n
        return
    }
    menubutton $button -text $n -menu ${button}.m -relief raised -border 1
    irmenu ${button}.m
    ${button}.m configure -tearoff off
    for {set i 0} {$i < $l} {incr i 2} {
        ${button}.m add command -label [lindex $names $i] \
                -command [list listbuttonv-action $button $var $names $i]
    }
}

# Procedure query-add-index-action {queryNo}
#  queryNo       query type number (integer)
# Handler that makes a new query index.
proc query-add-index-action {queryNo} {
    set w .query-setup

    global queryInfoTmp queryButtonsTmp

    set newI [.query-add-index.top.index.entry get]
    lappend queryInfoTmp [list $newI {}]
    $w.top.index.list insert end $newI
    destroy .query-add-index
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
}

# Procedure query-add-line
#  queryNo      query type number (integer)
# Handler that adds new query line.
proc query-add-line {queryNo} {
    global queryInfoTmp queryButtonsTmp
    
    set w .query-setup

    lappend queryButtonsTmp {I 0}
    
    set height [expr [winfo height $w] + 100]
#    set windowGeometry($w) ${height}x[winfo width $w]+0+0
    $w configure -height $height -width [winfo width $w]

    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
}

# Procedure query-del-line
#  queryNo      query type number (integer)
# Handler that removes query line.
proc query-del-line {queryNo} {
    set w .query-setup

    global queryInfoTmp queryButtonsTmp

    set l [llength $queryButtonsTmp]
    if {$l <= 0} {
        return
    }
    incr l -1
    set queryButtonsTmp [lreplace $queryButtonsTmp $l $l]
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
}

# Procedure query-add-index
#  queryNo      query type number (integer)
# Handler that adds new query index.
proc query-add-index {queryNo} {
    set w .query-add-index

    toplevel $w
    set oldFocus [focus]
    place-force $w .query-setup
    top-down-window $w
    frame $w.top.index
    pack $w.top.index -side top -anchor e -pady 2 
    entry-fields $w.top {index} {{Index Name:}} \
            [list query-add-index-action $queryNo] [list destroy $w]
    top-down-ok-cancel $w [list query-add-index-action $queryNo] 1
    focus $oldFocus
}

# Procedure query-setup-action
#  queryNo      query type number (integer)
# Handler that updates the query information database stored in the
# globals $queryInfo and $queryButtons. This procedure is executed when
# the user commits the query setup changes by pressing button "Ok".
proc query-setup-action {queryNo} {
    global queryButtonsTmp queryInfoTmp queryButtonsFind \
        queryInfoFind settingsChanged hostid currentDb profile attributeTypeSelected
    global queryInfo$attributeTypeSelected queryButtons$attributeTypeSelected
    set settingsChanged 1
    set queryInfo$attributeTypeSelected [lreplace [set queryInfo$attributeTypeSelected] \
        $queryNo $queryNo $queryInfoTmp]
    set queryButtons$attributeTypeSelected [lreplace \
        [set queryButtons$attributeTypeSelected] $queryNo $queryNo $queryButtonsTmp]
    if {[info exists profile($hostid,AttributeDetails,$currentDb,Bib1)]} {
        set profile($hostid,queryButtons,$currentDb) $queryButtonsTmp
    }
    set queryInfoFind $queryInfoTmp
    set queryButtonsFind $queryButtonsTmp
    destroy .query-setup
    index-lines .lines 1 $queryButtonsFind $queryInfoFind activate-index
}

#This procedure handles selection of what attribute set the user wants to use for searching.
#queryNo    index in the attributeTypes list (also the menu item number in Query|Type).
proc attribute-select {queryNo} {
    global attributeTypes attributeTypeSelected
    set attributeTypeSelected [lindex $attributeTypes $queryNo]
    foreach type $attributeTypes {
        global attribute[set type]
        set attribute$type 0
    }
    set attribute$attributeTypeSelected 1
}

#proc changeQueryButtons {target base}
#target        target name
#base        database name
#Substitutes the third element (the Auto element) in queryButtons with 
#profile(target,queryButtons,base). The third element in queryInfo is also substituted with
#profile(target,AttributeDetails,base,attributeTypeSelected)
proc changeQueryButtons {target base} {
    global profile queryInfo attributeTypeSelected queryTypes
    global queryButtons$attributeTypeSelected
    set n [lsearch $queryTypes Auto]
    if {[info exists profile($target,queryButtons,$base)]} {
        set queryButtons$attributeTypeSelected [lreplace [set \
            queryButtons$attributeTypeSelected] $n $n $profile($target,queryButtons,$base)]
        change-queryInfo $target $base
    }
}

# Procedure activate-e-index {value no i}
#   value   menu name
#   no      query index number
#   i       menu index (integer)
# Procedure called when listbutton is activated in the query type edit
# window. The global $queryButtonsTmp is updated in this operation.
proc activate-e-index {value no i} {
    global queryButtonsTmp queryIndexTmp
    set queryButtonsTmp [lreplace $queryButtonsTmp $no $no [list I $i]]
    dputs $queryButtonsTmp
    set queryIndexTmp $i
}

# Procedure activate-index {value no i}
#   value   menu name
#   no      query index number
#   i       menu index (integer)
# Procedure called when listbutton is activated in the main query 
# window. The global $queryButtonsFind is updated in this operation.
proc activate-index {value no i} {
    global queryButtonsFind

    set queryButtonsFind [lreplace $queryButtonsFind $no $no [list I $i]]
    dputs "queryButtonsFind $queryButtonsFind"
}

# Procedure update-attr
# This procedure creates listbuttons for all bib-1 attributes except
# the use-attribute in the .index-setup window.
# The globals $relationTmpValue, $positionTmpValue, $structureTmpValue,
# $truncationTmpValue and $completenessTmpValue are maintainted by the
# listbuttons.
proc update-attr {} {
    set w .index-setup
    listbuttonv $w.top.relation.b relationTmpValue\
        {{None} 0 {Less than} 1 {Greater than or equal} 2 {Equal} 3 \
        {Greater than or equal} 4 {Greater than} 5 {Not equal} 6 \
        {Phonetic} 100 {Stem} 101 {Relevance} 102 {AlwaysMatches} 103}
    listbuttonv $w.top.position.b positionTmpValue {{None} 0 \
        {First in field} 1 {First in subfield} 2 {Any position in field} 3}
    listbuttonv $w.top.structure.b structureTmpValue {{None} 0 {Phrase} 1 \
        {Word} 2 {Key} 3 {Year} 4 {Date (norm)} 5 {Word list}  6 \
        {Date (un-norm)} 100 {Name (norm)} 101 {Date (un-norm)} 102 \
        {Structure} 103 {urx} 104 {free-form} 105 {doc-text} 106 \
        {local-number} 107 {string} 108 {numeric string} 109}
    listbuttonv $w.top.truncation.b truncationTmpValue {{Auto} 0 {Right} 1 \
        {Left} 2 {Left and right} 3 {No truncation} 100 \
        {Process #} 101 {Re-1} 102 {Re-2} 103}
    listbuttonv $w.top.completeness.b completenessTmpValue {{None} 0 \
        {Incomplete subfield} 1 {Complete subfield} 2 {Complete field} 3}
}

# Procedure use-attr {init}
#  init      init flag
# This procedure creates a listbox with several Bib-1 use attributes.
# If $init is 1 the listbox is created with the attributes. If $init
# is 0 the current selection of the listbox is read and the global
# $useTmpValue is set to the current use-value.
proc use-attr {init} {
    global useTmpValue attributeTypeSelected queryAuto profile hostid currentDb
    set ats [string tolower $attributeTypeSelected]
    source ${ats}.tcl
    
    set w .index-setup

    if {$init} {
        set s 0
        set lno 0
        if {$queryAuto} {
            foreach i $profile($hostid,AttributeDetails,$currentDb,$attributeTypeSelected) {
                $w.top.use.list insert end "[set ${ats}($i)]"
                if {$useTmpValue == $i} {
                    set s $lno
                }
                incr lno
            }
        } else {
            foreach i [lsort -integer [array names $ats]] {
                $w.top.use.list insert end "[set ${ats}($i)]"
                if {$useTmpValue == $i} {
                    set s $lno
                }
                incr lno
            }
        }
        $w.top.use.list selection clear 0 end
        $w.top.use.list selection set $s $s
#        incr s -3
#        if {$s < 0} 
#            set s 0
#        
        $w.top.use.list yview $s
    } else {
        set j [lindex [$w.top.use.list curselection] 0]
        set i [lindex [lsort -integer [array names ${ats}]] $j]
#        debug-window "[$w.top.use.list curselection] [set ${ats}($i)]"
#        set useTmpValue [set ${ats}($i)]
        set useTmpValue $i
        dputs "useTmpValue=$useTmpValue"
    }
}

# Procedure index-setup-action {oldAttr queryNo indexNo}
#  oldAttr     original attributes (?)
#  queryNo     query number
#  indexNo     index number
# Commits setup of a query index. The mapping from the index to 
# the Bib-1 attributes are handled by this function.
proc index-setup-action {oldAttr queryNo indexNo} {
    set attr [lindex $oldAttr 0]

    global useTmpValue relationTmpValue structureTmpValue truncationTmpValue \
        completenessTmpValue positionTmpValue queryInfoTmp

    use-attr 0

    dputs "index-setup-action"
    dputs "queryNo $queryNo"
    dputs "indexNo $indexNo"
    if {$useTmpValue > 0} {
        lappend attr "1=$useTmpValue"
    }
    if {$relationTmpValue > 0} {
        lappend attr "2=$relationTmpValue"
    }
    if {$positionTmpValue > 0} {
        lappend attr "3=$positionTmpValue"
    }
    if {$structureTmpValue > 0} {
        lappend attr "4=$structureTmpValue"
    }
    if {$truncationTmpValue > 0} {
        lappend attr "5=$truncationTmpValue"
    }
    if {$completenessTmpValue > 0} {
        lappend attr "6=$completenessTmpValue"
    }
    dputs "new attr $attr"
    set queryInfoTmp [lreplace $queryInfoTmp $indexNo $indexNo $attr]
    destroy .index-setup
}

# Procedure index-setup {attr queryNo indexNo}
#  attr        original attributes
#  queryNo     query number
#  indexNo     index number
# Makes a window with settings of a given query index which the user
# may inspect/modify.
proc index-setup {attr queryNo indexNo} {
    set w .index-setup

    global relationTmpValue structureTmpValue truncationTmpValue \
        completenessTmpValue positionTmpValue useTmpValue attributeTypeSelected
    set relationTmpValue 0
    set truncationTmpValue 0
    set structureTmpValue 0
    set positionTmpValue 0
    set completenessTmpValue 0
    set useTmpValue 0

    catch {destroy $w}
    toplevelG $w

    set n [lindex $attr 0]
    wm title $w "Index setup: $n, $attributeTypeSelected"

    top-down-window $w

    set len [llength $attr]
    for {set i 1} {$i < $len} {incr i} {
        set q [lindex $attr $i]
        set l [string first = $q]
        if {$l > 0} {
            set t [string range $q 0 [expr {$l - 1}]]
            set v [string range $q [expr {$l + 1}] end]
            switch $t {
                1
                { set useTmpValue $v }
                2
                { set relationTmpValue $v }
                3
                { set positionTmpValue $v }
                4
                { set structureTmpValue $v }
                5
                { set truncationTmpValue $v }
                6
                { set completenessTmpValue $v }
            }
        }
    }

    frame $w.top.use -relief ridge -border 2
    frame $w.top.relation -relief ridge -border 2
    frame $w.top.position -relief ridge -border 2
    frame $w.top.structure -relief ridge -border 2
    frame $w.top.truncation -relief ridge -border 2
    frame $w.top.completeness -relief ridge -border 2

    update-attr

    # Use Attributes

    pack $w.top.use -side left -pady 6 -padx 6 -fill y

    label $w.top.use.label -text "Use"
    listbox $w.top.use.list -width 26 -yscrollcommand "$w.top.use.scroll set"
    scrollbar $w.top.use.scroll -orient vertical -border 1
    pack $w.top.use.label -side top -fill x -padx 2 -pady 2
    pack $w.top.use.list -side left -fill both -expand yes -padx 2 -pady 2
    pack $w.top.use.scroll -side right -fill y -padx 2 -pady 2
    $w.top.use.scroll config -command "$w.top.use.list yview"

    use-attr 1

    # Relation Attributes

    pack $w.top.relation -pady 6 -padx 6 -side top
    label $w.top.relation.label -text "Relation" -width 18
    
    pack $w.top.relation.label $w.top.relation.b -fill x 

    # Position Attributes

    pack $w.top.position -pady 6 -padx 6 -side top
    label $w.top.position.label -text "Position" -width 18

    pack $w.top.position.label $w.top.position.b -fill x

    # Structure Attributes

    pack $w.top.structure -pady 6 -padx 6 -side top
    label $w.top.structure.label -text "Structure" -width 18

    pack $w.top.structure.label $w.top.structure.b -fill x

    # Truncation Attributes

    pack $w.top.truncation -pady 6 -padx 6 -side top
    label $w.top.truncation.label -text "Truncation" -width 18

    pack $w.top.truncation.label $w.top.truncation.b -fill x

    # Completeness Attributes

    pack $w.top.completeness -pady 6 -padx 6 -side top
    label $w.top.completeness.label -text "Completeness" -width 18

    pack $w.top.completeness.label $w.top.completeness.b -fill x

    # Ok-cancel
    bottom-buttons $w [list {Ok} \
        [list index-setup-action $attr $queryNo $indexNo] {Cancel} [list destroy $w]] 0
    
    focus $w
}

# Procedure query-edit-index {queryNo}
#  queryNo     query number
# Determines if a selection of an index is active. If one is selected
# the index-setup dialog is started.
proc query-edit-index {queryNo} {
    global queryInfoTmp
    set w .query-setup

    set i [lindex [$w.top.index.list curselection] 0]
    if {![string length $i]} {
        return
    }
    set attr [lindex $queryInfoTmp $i]
    dputs "Editing no $i $attr"
    index-setup $attr $queryNo $i
}

# Procedure query-delete-index {queryNo}
#  queryNo     query number
# Determines if a selection of an index is active. If one is selected
# the index is deleted.
proc query-delete-index {queryNo} {
    global queryInfoTmp queryButtonsTmp
    set w .query-setup

    set i [lindex [$w.top.index.list curselection] 0]
    if {![string length $i]} {
        return
    }
    set queryInfoTmp [lreplace $queryInfoTmp $i $i]
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
    $w.top.index.list delete $i
}
    
# Procedure query-setup {queryNo}
#  queryNo     query number
# Makes a dialog in which a query type can be customized.
proc query-setup {queryNo} {
    global queryTypes queryButtonsTmp queryInfoTmp queryIndexTmp attributeTypeSelected
    global queryInfo$attributeTypeSelected queryButtons$attributeTypeSelected
    set w .query-setup

    set queryIndexTmp 0
    set queryName [lindex $queryTypes $queryNo]
    if {[info exists queryInfo$attributeTypeSelected]} {
        set queryInfoTmp [lindex [set queryInfo$attributeTypeSelected] $queryNo]
    } else {
        set queryInfoTmp [lindex $queryInfoBib1 $queryNo]
    }
    if {[info exists queryButtons$attributeTypeSelected]} {
        set queryButtonsTmp [lindex [set queryButtons$attributeTypeSelected] $queryNo]
    } else {
        set queryButtonsTmp [lindex $queryButtonsBib1 $queryNo]
    }

    toplevelG $w

    wm minsize $w 0 0
    wm title $w "Query setup $queryName - $attributeTypeSelected"

    top-down-window $w

    frame $w.top.lines -relief ridge -border 2
    pack $w.top.lines -side left -pady 6 -padx 6 -fill y

    # Index Lines

    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index

    button $w.top.lines.add -text "Add" -command [list query-add-line $queryNo]
    button $w.top.lines.del -text "Remove" -command [list query-del-line $queryNo]
    pack $w.top.lines.del -fill x -side bottom
    pack $w.top.lines.add -fill x -pady 10 -side bottom

    # Indexes

    frame $w.top.index -relief ridge -border 2
    pack $w.top.index -pady 6 -padx 6 -side right -fill y

    listbox $w.top.index.list -yscrollcommand [list $w.top.index.scroll set] \
        -background white
    scrollbar $w.top.index.scroll -orient vertical -border 1 \
        -command [list $w.top.index.list yview]
    bind $w.top.index.list <Double-1> [list query-edit-index $queryNo]

    pack $w.top.index.list -side left -fill both -expand yes -padx 2 -pady 2
    pack $w.top.index.scroll -side right -fill y -padx 2 -pady 2

    $w.top.index.list selection clear 0 end
    $w.top.index.list selection set 0 0
    foreach x $queryInfoTmp {
        $w.top.index.list insert end [lindex $x 0]
    }

    # Bottom
    bottom-buttons $w [list \
            Ok [list query-setup-action $queryNo] \
            Add [list query-add-index $queryNo] \
            Edit [list query-edit-index $queryNo] \
            Delete [list query-delete-index $queryNo] \
            Cancel [list destroy $w]] 0
    focus $w
}

# Procedure index-clear
# Handler that clears the search entry fields.
proc index-clear {} {
    global queryButtonsFind

    set i 0
    foreach b $queryButtonsFind {
        .lines.$i.e delete 0 end
        incr i
    }
}

# Procedure index-query
# The purpose of this function is to read the user's query and convert
# it to the prefix query that IrTcl/YAZ uses to represent an RPN query.
# Each entry in a search fields takes the form
#    [relOp][?]term[?]
#  Here, relOp is an optional relational operator and one of:
#      >  < >= <=  <>
#    which sets the Bib-1 relation to greater-than, less-than, etc.
#  The ? (question-mark) is also optional. A (?) on left-side indicates
#    left truncation; (?) on right-side indicates right-truncation; (?)
#    on both sides indicates both-left-and-right truncation.
proc index-query {} {
    global queryButtonsFind queryInfoFind attributeTypeSelected

    set i 0
    set qs {}

    foreach b $queryButtonsFind {
        set term [string trim [.lines.$i.e get]]
        if {$term != ""} {
            set attr [lrange [lindex $queryInfoFind [lindex $b 1]] 1 end]

            set relation ""
            set len [string length $term]
            incr len -1

            if {$len > 1} {
                if {[string index $term 0] == ">"} {
                    if {[string index $term 1] == "=" } {
                        set term [string trim [string range $term 2 $len]]
                        set relation 4
                    } else {
                        set term [string trim [string range $term 1 $len]]
                        set relation 5
                    }
                } elseif {[string index $term 0] == "<"} {
                    if {[string index $term 1] == "=" } {
                        set term [string trim [string range $term 2 $len]]
                        set relation 2
                    } elseif {[string index $term 1] == ">"} {
                        set term [string trim [string range $term 2 $len]]
                        set relation 6
                    } else {
                        set term [string trim [string range $term 1 $len]]
                        set relation 1
                    }
                }
            } 
            set len [string length $term]
            incr len -1
            set left 0
            set right 0
            if {[string index $term $len] == "?"} {
                set right 1
                set term [string range $term 0 [expr {$len - 1}]]
            }
            if {[string index $term 0] == "?"} {
                set left 1
                set term [string range $term 1 end]
            }
            set term "\{${term}\}"
            if {$right && $left} {
                set term "@attr 5=3 ${term}"
            } elseif {$right} {
                set term "@attr 5=1 ${term}"
            } elseif {$left} {
                set term "@attr 5=2 ${term}"
            }
            if {$relation != ""} {
                set term "@attr 2=${relation} ${term}"
            }
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
#    debug-window "Querystring er her $qs"
    if {$qs == ""} {
        return ""
    } else {
        set qs "@attrset $attributeTypeSelected $qs"
        dputs "qs=  $qs"
#        debug-window "....og nu er den $qs\n"
        return $qs
    }
}

# Procedure index-focus-in {w i}
#  w    index frame
#  i    index number
# This procedure handles <FocusIn> events. A red border is drawed
# around the active search entry field.
proc index-focus-in {w i} {
    global curIndexEntry
    $w.$i configure -background red
    set curIndexEntry $i
}

# Procedure index-lines {w readOp buttonInfo queryInfo handle}
#  w          search fields entry frame
#  realOp     if true, search-request bindings are bound to the entries.
#  buttonInfo query type button information
#  queryInfo  query type field information
#  handle     handler called a when a 'listbutton' changes its value
# Makes one or more search areas - with listbuttons on the left
# and entries on the right. 
proc index-lines {w realOp buttonInfo queryInfo handle} {
    set i 0
    foreach b $buttonInfo {
        if {! [winfo exists $w.$i]} {
            frame $w.$i -border 0
        }
        listbuttonx $w.$i.l [lindex $b 1] $queryInfo $handle $i

        if {$realOp} {
            if {! [winfo exists $w.$i.e]} {
                entry $w.$i.e -width 32 -relief sunken -border 1
                bind $w.$i.e <FocusIn> [list index-focus-in $w $i]
                bind $w.$i.e <FocusOut> [list $w.$i configure -background white]
                pack $w.$i.l -side left
                pack $w.$i.e -side left -fill x -expand yes
                pack $w.$i -side top -fill x -padx 2 -pady 2
                bind $w.$i.e <Left> [list left-cursor $w.$i.e]
                bind $w.$i.e <Right> [list right-cursor $w.$i.e]
                bind $w.$i.e <Return> {search-request 0}
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
        set k [expr {$j + 1}]
        bind $w.$j.e <Tab> "focus $w.$k.e"
        set j $k
    }
    if {$i >= 0} {
        bind $w.$i.e <Tab> "focus $w.0.e"
        focus $w.0.e
    }
}

#Procedure configureOptionsSyntax {target base}
#target        target name
#base         database name
#Changes the Options|Syntax menu acording to the information obtained via explain.
proc configureOptionsSyntax {target base} {
    global profile syntaxList recordSyntax 
    set activate 0
    set i -1
    set j 0
    set w .top.options.m.syntax
    if {[info exists profile($target,RecordSyntaxes,$base)]} {
        foreach syntax $syntaxList {
            incr i
            if {$syntax == "sep"} {continue}
            incr j
            if {[lsearch $profile($target,RecordSyntaxes,$base) $syntax] != -1} {
                configure-enable-e $w $i
                if {$activate == 0} {
                    $w invoke $j
                    set recordSyntax $syntax
                    set activate 1
                }
            } else {
                configure-disable-e $w $i
            }
        }
    } else {
        foreach syntax $syntaxList {
            incr i
            if {$syntax == "sep"} {continue}
            incr j
            if {$syntax == $recordSyntax} {
                $w invoke $j
            }
            configure-enable-e $w $i
        }
    }
}

# Init: The geometry information for the main window is set - either
# to a default value or to the value in windowGeometry(.)
if {[catch {set g $windowGeometry(.)}]} {
    wm geometry . 500x410
} else {
    wm geometry . $g
}

# Init: Presentation formats are read.
read-formats

# Init: The main window is defined.
frame .top  -border 1 -relief raised
frame .lines  -border 1 -relief raised
frame .mid  -border 1 -relief raised
frame .data -border 1 -relief raised
frame .bot  -border 1 -relief raised
pack .top .lines .mid -side top -fill x
pack .data -side top -fill both -expand yes
pack .bot -fill x
#irmenu .top.file

# Init: Definition of File menu.
menubutton .top.file -text File -menu .top.file.m -underline 0
irmenu .top.file.m
.top.file.m add command -label {Save settings} -command {save-settings}
.top.file.m add separator
.top.file.m add command -label Exit -command {exit-action}

# Init: Definition of Target menu.
menubutton .top.target -text Target -menu .top.target.m -underline 0
irmenu .top.target.m
.top.target.m add cascade -label Connect -menu .top.target.m.clist
.top.target.m add command -label Disconnect -command {close-target}
.top.target.m add command -label About -command {about-target}
.top.target.m add cascade -label Setup -menu .top.target.m.slist
.top.target.m add command -label {Setup new} -command {define-target-dialog}
.top.target.m add separator
set-target-hotlist 0

configure-disable-e .top.target.m 1
configure-disable-e .top.target.m 2

irmenu .top.target.m.clist
irmenu .top.target.m.slist
cascade-target-list

# Init: Definition of Service menu.
menubutton .top.service -text Service -menu .top.service.m -underline 0
irmenu .top.service.m
.top.service.m add cascade -label Database -menu .top.service.m.dblist
.top.service.m add cascade -label Present -menu .top.service.m.present
irmenu .top.service.m.present
.top.service.m.present add command -label {10 More} -command [list present-more 10]
.top.service.m.present add command -label All -command [list present-more {}]
.top.service.m add command -label Search -command {search-request 0}
.top.service.m add command -label Scan -command {scan-request}
.top.service.m add command -label Explain -command \
    {explain-refresh $hostid {ready-response {}} }

.top.service configure -state disabled

irmenu .top.service.m.dblist

# Init: Definition of Set menu.
menubutton .top.rset -text Set -menu .top.rset.m -underline 1
irmenu .top.rset.m
.top.rset.m add command -label Load -command {load-set}
.top.rset.m add separator

# Init: Definition of the Options menu.
menubutton .top.options -text Options -menu .top.options.m -underline 0
irmenu .top.options.m
.top.options.m add cascade -label Query -menu .top.options.m.query
.top.options.m add cascade -label Format -menu .top.options.m.formats
.top.options.m add cascade -label Wrap -menu .top.options.m.wrap
.top.options.m add cascade -label Syntax -menu .top.options.m.syntax
.top.options.m add cascade -label Elements -menu .top.options.m.elements
.top.options.m add radiobutton -label Debug -variable debugMode -value 1

# Init: Definition of the Options|Query menu.
irmenu .top.options.m.query
.top.options.m.query add cascade -label Select -menu .top.options.m.query.clist
.top.options.m.query add cascade -label Type -menu .top.options.m.query.tlist
.top.options.m.query add cascade -label Edit -menu .top.options.m.query.slist
.top.options.m.query add command -label New -command {query-new}
.top.options.m.query add cascade -label Delete -menu .top.options.m.query.dlist

irmenu .top.options.m.query.slist
irmenu .top.options.m.query.tlist
irmenu .top.options.m.query.clist
irmenu .top.options.m.query.dlist
cascade-query-list

# Init: Definition of the Options|Formats menu.
irmenu .top.options.m.formats
set i 0
foreach f $displayFormats {
    .top.options.m.formats add radiobutton -label $f -value $i \
            -command [list set-display-format $i] -variable displayFormat
    incr i
}

# Init: Definition of the Options|Wrap menu.
irmenu .top.options.m.wrap
.top.options.m.wrap add radiobutton -label Character \
        -value char -variable textWrap -command {set-wrap char}
.top.options.m.wrap add radiobutton -label Word \
        -value word -variable textWrap -command {set-wrap word}
.top.options.m.wrap add radiobutton -label None \
        -value none -variable textWrap -command {set-wrap none}

# Init: Definition of the Options|Syntax menu.
proc initOptionsSyntax {} {
    global syntaxList recordSyntax
    set w .top.options.m.syntax
    irmenu $w
    foreach syntax $syntaxList {
        if {$syntax == "sep"} {
            $w add separator
        } else {
            $w add radiobutton -label $syntax -value $syntax -variable recordSyntax
        }
    }
}
initOptionsSyntax

# Init: Definition of the Options|Elements menu.
irmenu .top.options.m.elements
.top.options.m.elements add radiobutton -label Unspecified \
        -value None -variable elementSetNames
.top.options.m.elements add radiobutton -label Full -value F -variable elementSetNames
.top.options.m.elements add radiobutton -label Brief -value B -variable elementSetNames

# Init: Definition of Help menu.
menubutton .top.help -text "Help" -menu .top.help.m -underline 0
irmenu .top.help.m

#.top.help.m add command -label "Help on help" -command {tkerror "Help on help not available. Sorry"}
.top.help.m add command -label "Help on help" -command {bgerror "Help on help not available. Sorry"}
.top.help.m add command -label "About" -command {about-origin}

# Init: Pack menu bar items.
pack .top.file .top.target .top.service .top.rset .top.options -side left
pack .top.help -side right
#.top configure -menu .top.file

# Init: Define query area buttons with icons.
index-lines .lines 1 $queryButtonsFind [lindex [set \
    queryInfo$attributeTypeSelected] 0] activate-index
image create photo scan -file [file join $libdir bitmaps a-z.gif]
image create photo clear -file [file join $libdir bitmaps trash.gif]
image create photo present -file [file join $libdir bitmaps page.gif]
image create photo search -file [file join $libdir bitmaps search.gif]
image create photo stop -file [file join $libdir bitmaps stop.gif]
button .mid.search -image search -command {search-request 0} \
    -state disabled -relief flat
button .mid.scan -image scan -command scan-request -state disabled -relief flat
button .mid.present -image present -command [list present-more 10] \
    -state disabled -relief flat
button .mid.clear -image clear -command index-clear -relief flat
button .mid.stop -image stop -command cancel-operation -relief flat
pack .mid.search .mid.scan .mid.present .mid.clear -side left -fill y -pady 1
pack .mid.stop -side left -fill y -padx 20

# Init: Define record area in main window.
text .data.record -font fixed -height 2 -width 20 -wrap none -borderwidth 0 \
    -relief flat -yscrollcommand [list .data.scroll set] -wrap $textWrap
scrollbar .data.scroll -command [list .data.record yview]
.data.record configure -takefocus 0
.data.scroll configure -takefocus 0

pack .data.scroll -side right -fill y
pack .data.record -expand yes -fill both
initBindings

# Init: Define standards tags. These are used in the display
# format procedures.

.data.record tag configure marc-tag -foreground blue
.data.record tag configure marc-id -foreground red
.data.record tag configure marc-data -foreground black
.data.record tag configure marc-head -font $font(n,normal) \
        -foreground brown -relief raised -borderwidth 1
.data.record tag configure marc-small-head -foreground brown
.data.record tag configure marc-pref -font $font(n,normal) -foreground blue
.data.record tag configure marc-text -font $font(n,normal) -foreground black
.data.record tag configure marc-it -font $font(n,normal) -foreground black

# Init: Define logo.
button .bot.logo -bitmap @[file join $libdir bitmaps book1] -command cancel-operation
.bot.logo configure -takefocus 0

# Init: Define status information fields at the bottom.
frame .bot.a
pack .bot.a -side left -fill x
pack .bot.logo -side right -padx 2 -pady 2 -ipadx 1

message .bot.a.target -text {} -aspect 2000 -border 1

label .bot.a.status -text "Not connected" -width 15 -relief sunken -anchor w -border 1
label .bot.a.set -text "" -width 5 -relief sunken -anchor w -border 1
label .bot.a.message -text "" -width 15 -relief sunken -anchor w -border 1

pack .bot.a.target -side top -anchor nw -padx 2 -pady 2
pack .bot.a.status .bot.a.set .bot.a.message -side left -padx 2 -pady 2 -ipadx 1 -ipady 1

# Init: Determine if the IrTcl extension is already there. If
#  not, then dynamically load the IrTcl extension.
if {[catch {ir z39}]} {
    set e [info sharedlibextension]
    puts -nonewline "Loading irtcl$e ..."
    load [file join $libdir irtcl$e] irtcl
    ir z39
    puts "ok"
}

if {[file exists [file join $libdir explain.tcl]]} {
    source [file join $libdir explain.tcl]
}

#if {[file exists ${libdir}/setup.tcl]} 
    source [file join $libdir setup.tcl]


# Init: Uncomment this line if you wan't to enable logging.
ir-log-init all irtcl irtcl.log

# Init: If hostid is a valid target, a new connection will be established
# immediately.
if {[string compare $hostid Default]} {
    catch {open-target $hostid $hostbase}
}

# Init: Enable the logo.
show-logo 1


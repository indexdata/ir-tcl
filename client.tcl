# IR toolkit for tcl/tk
# (c) Index Data 1995-1996
# See the file LICENSE for details.
# Sebastian Hammer, Adam Dickmeiss
#
# $Log: client.tcl,v $
# Revision 1.99  1997-04-13 19:00:37  adam
# Added support for Tcl8.0/Tk8.0.
# New command ir-log-init to setup yaz logging facilities.
#
# Revision 1.98  1996/11/14 17:11:04  adam
# Added Explain documentaion.
#
# Revision 1.97  1996/09/13  10:54:22  adam
# Started work on Explain in client.
#
# Revision 1.96  1996/08/09  15:30:18  adam
# Procedure destroyGW modified to handle multiple calls - probably an
# error introduced by tk4.1 patch level 1.
#
# Revision 1.95  1996/07/26  09:15:08  adam
# IrTcl version 1.2 patch level 1.
#
# Revision 1.94  1996/07/25  15:55:34  adam
# IrTcl version 1.2 release.
#
# Revision 1.93  1996/06/28  08:43:54  adam
# Moved towards version 1.2.
#
# Revision 1.92  1996/03/29  16:04:30  adam
# Work on GRS-1 presentation.
#
# Revision 1.91  1996/03/27  17:00:53  adam
# Fix: main defined when using Tk3.6; it shouldn't be.
#
# Revision 1.90  1996/03/20  13:54:02  adam
# The Tcl_File structure is only manipulated in the Tk-event interface
# in tkinit.c.
#
# Revision 1.89  1996/03/05  09:16:04  adam
# Sets tearoff to off on several menus.
#
# Revision 1.88  1996/01/23  15:24:09  adam
# Wrore more comments.
#
# Revision 1.87  1996/01/22  17:13:34  adam
# Wrote comments.
#
# Revision 1.86  1996/01/22  09:29:01  adam
# Wrote comments.
#
# Revision 1.85  1996/01/19  16:22:36  adam
# New method: apduDump - returns information about last incoming APDU.
#
# Revision 1.84  1996/01/11  13:12:10  adam
# Bug fix.
#
# Revision 1.83  1995/11/28  17:26:36  adam
# Removed Carriage return from ir-tcl.c!
# Removed misc. debug logs.
#
# Revision 1.82  1995/11/02  08:47:56  adam
# Text widgets are flat now.
#
# Revision 1.81  1995/10/19  10:34:43  adam
# More configurable client.
#
# Revision 1.80  1995/10/18  17:20:32  adam
# Work on target setup in client.tcl.
#
# Revision 1.79  1995/10/18  16:42:37  adam
# New settings: smallSetElementSetNames and mediumSetElementSetNames.
#
# Revision 1.78  1995/10/18  15:45:36  quinn
# *** empty log message ***
#
# Revision 1.77  1995/10/18  15:37:46  adam
# Piggy-back present.
#
# Revision 1.76  1995/10/18  15:15:20  adam
# Fixed bug.
#
# Revision 1.75  1995/10/17  14:18:05  adam
# Minor changes in presentation formats.
#
# Revision 1.74  1995/10/17  12:18:57  adam
# Bug fix: when target connection closed, the connection was not
# properly reestablished.
#
# Revision 1.73  1995/10/17  10:58:06  adam
# More work on presentation formats.
#
# Revision 1.72  1995/10/16  17:00:52  adam
# New setting: elementSetNames.
# Various client improvements. Medium presentation format looks better.
#
# Revision 1.71  1995/10/13  15:35:27  adam
# Relational operators may be used in search entries - changes
# in proc index-query.
#
# Revision 1.70  1995/10/12  14:46:52  adam
# Better record popup windows. Next/prev buttons in popup record windows.
# The record position in the raw format is much more visible.
#
# Revision 1.69  1995/09/21  13:42:54  adam
# Bug fixes.
#
# Revision 1.68  1995/09/21  13:11:49  adam
# Support of dynamic loading.
# Test script uses load command if necessary.
#
# Revision 1.67  1995/09/20  14:35:19  adam
# Minor changes.
#
# Revision 1.66  1995/08/29  15:30:13  adam
# Work on GRS records.
#
# Revision 1.65  1995/08/24  15:39:09  adam
# Minor changes.
#
# Revision 1.64  1995/08/24  15:33:02  adam
# Minor changes.
#
# Revision 1.63  1995/08/04  13:20:48  adam
# Buttons at the bottom are slightly smaller.
#
# Revision 1.62  1995/08/04  11:32:37  adam
# More work on output queue. Memory related routines moved
# to mem.c
#
# Revision 1.61  1995/07/20  08:09:39  adam
# client.tcl: Targets removed from hotTargets list when targets
#  are removed/modified.
# ir-tcl.c: More work on triggerResourceControl.
#
# Revision 1.60  1995/06/30  16:30:19  adam
# Minor changes.
#
# Revision 1.59  1995/06/29  14:06:25  adam
# Another bug in install fixed. Configure searches for more versions of yaz.
#
# Revision 1.58  1995/06/29  12:34:06  adam
# IrTcl now works with both tk4.0b4/tcl7.4b4 and tk3.6/tcl7.3
#
# Revision 1.57  1995/06/29  09:20:30  adam
# Target entries in cascade menus are sorted.
#
# Revision 1.56  1995/06/27  19:03:48  adam
# Bug fix in do_present in ir-tcl.c: p->set_child member weren't set.
# nextResultSetPosition used instead of setOffset.
#
# Revision 1.55  1995/06/27  17:10:37  adam
# Bug fix: install procedure didn't work on some systems.
# Error turned up when clientrc.tcl was't present.
#
# Revision 1.54  1995/06/27  14:41:03  adam
# Bug fix in search-response. Didn't always observe non-surrogate diagnostics.
#
# Revision 1.53  1995/06/26  12:40:09  adam
# Client defines its own tkerror.
# User may specify 'no preferredRecordSyntax'.
#
# Revision 1.52  1995/06/22  13:14:59  adam
# Feature: SUTRS. Setting getSutrs implemented.
# Work on display formats.
# Preferred record syntax can be set by the user.
#
# Revision 1.51  1995/06/21  11:11:00  adam
# Bug fix: libdir undefined in about-origin.
#
# Revision 1.50  1995/06/21  11:04:48  adam
# Uses GNU autoconf 2.3.
# Install procedure implemented.
# boook bitmaps moved to sub directory bitmaps.
#
# Revision 1.49  1995/06/20  14:16:42  adam
# More work on cancel mechanism.
#
# Revision 1.48  1995/06/20  08:07:23  adam
# New setting: failInfo.
# Working on better cancel mechanism.
#
# Revision 1.47  1995/06/19  14:05:29  adam
# Bug fix: asked for SUTRS.
#
# Revision 1.46  1995/06/19  13:06:06  adam
# New define: IR_TCL_VERSION.
#
# Revision 1.45  1995/06/19  08:08:44  adam
# client.tcl: hotTargets now contain both database and target name.
# ir-tcl.c: setting protocol edited. Errors in callbacks are logged
# by logf(LOG_WARN, ...) calls.
#
# Revision 1.44  1995/06/16  14:55:18  adam
# Book logo mirrored.
#
# Revision 1.43  1995/06/16  14:41:05  adam
# Scan line entries can be copied to a search entry.
#
# Revision 1.42  1995/06/16  12:28:13  adam
# Implemented preferredRecordSyntax.
# Minor changes in diagnostic handling.
# Record list deleted when connection closes.
#
# Revision 1.41  1995/06/14  15:07:59  adam
# Bug fix in cascade-target-list. Uses yaz-version.h.
#
# Revision 1.40  1995/06/14  13:37:17  adam
# Setting recordType implemented.
# Setting implementationVersion implemented.
# Settings implementationId / implementationName edited.
#
# Revision 1.39  1995/06/14  12:16:22  adam
# hotTargets, textWrap and displayFormat saved in clientg.tcl.
#
# Revision 1.38  1995/06/14  07:22:45  adam
# Target definitions can be deleted.
# Listbox used in the query definition dialog.
#
# Revision 1.37  1995/06/13  14:37:59  adam
# Work on query setup.
# Better about origin/target.
# Better presentation formats.
#
# Revision 1.36  1995/06/13  07:42:14  adam
# Bindings removed from text widgets.
#
# Revision 1.35  1995/06/12  15:17:31  adam
# Text widget used in main window (instead of listbox) to support
# better presentation formats.
#
# Revision 1.34  1995/06/12  07:59:07  adam
# More work on geometry handling.
#
# Revision 1.33  1995/06/09  11:17:35  adam
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

# Procedure tk4 is defined - returns 0 if tk 3.6 - returns 1 otherwise
if {$tk_version == "3.6"} {
    proc tk4 {} {
        return 0
    }
} else {
    proc tk4 {} {
        return 1
    }
}

# The following two procedures deals with menu entries. The interface
# changed from Tk 3.6 to 4.X

# Procedure configure-enable-e {w n}
#  w   is a menu
#  n   menu entry number (0 is first entry)
# Enables menu entry

# Procedure configure-disable-e {w n}
#  w   is a menu
#  n   menu entry number (0 is first entry)
# Disables menu entry

if {[tk4]} {
    proc configure-enable-e {w n} {
        incr n
        $w entryconfigure $n -state normal
    }
    proc configure-disable-e {w n} {
        incr n
        $w entryconfigure $n -state disabled
    }
    set noFocus [list -takefocus 0]
} else {
    proc configure-enable-e {w n} {
        $w enable $n
    }
    proc configure-disable-e {w n} {
        $w disable $n
    }
    set noFocus {}
}

# Define dummy clock function if it is not there.
if {[catch {clock seconds}]} {
    proc clock {args} {
        return {}
    }
}
# Set monoFlag to 1 if screen is known not to support colors; otherwise
#  set monoFlag to 0
if {![tk4]} {
    if {[tk colormodel .] == "color"} {
        set monoFlag 0
    } else {
        set monoFlag 1
    }
} else {
    set monoFlag 0
}

# Define libdir to the IrTcl configuration directory.
# In the line below LIBDIR will be modified during 'make install'.
set libdir LIBDIR

# If the bitmaps sub directory is present with a bitmap we assume 
# the client is run from the source directory in which case we
# set libdir the current directory.
if {[file readable bitmaps/book2]} {
    set libdir .
}

# Make a final check to see if libdir was set ok.
if {! [file readable ${libdir}/bitmaps/book2]} {
    puts "Cannot locate system files in ${libdir}. You must either run this"
    puts "program from the source directory root of ir-tcl or you must assure"
    puts "that it is installed - normally in /usr/local/lib/irtcl"
    exit 1
}

# Initialize a lot of globals.
set hotTargets {}
set hotInfo {}
set busy 0

# profile: associative array with target profiles.
#indx exp description
#
#   0  T  Target description
#   1     Host
#   2     Port
#   3     Authentication
#   4     Maximum Record Size
#   5     Preferred Messages Size
#   6     Comstack
#   7  D  Databases available
#   8  T  Result Sets support
#   9     RPN-Query support
#  10     CCL-Query support
#  11     Protocol (Z39/SR)
#  12     Window Number
#  13     LSLB  Large Set Lower Bound
#  14     SSUB  Small Set Upper Bound
#  15     MSPN  Medium Set Present Number
#  16     Present Chunk - number of records to fetch in each present
#  17     Time of first define
#  18     Time of last init
#  19     Time of last explain
#  20  T  Name in TargetInfo
#  21  T  Recent News
#  22  T  Max Result Sets
#  23  T  Max Result Size
#  24  T  Max Terms
#  25  D  List of database info records
#  26  T  Multiple Databases
#  27  T  Welcome message
#
#
# Legend:
#  T  TargetInfo explain
#  D  DatabaseInfo explain

set profile(Default) {{} {} {210} {} 50000 30000 tcpip {} 1 {} {} Z39 1 2 0 0 4}
set hostid Default
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

set queryTypes {Simple}
set queryButtons { { {I 0} {I 1} {I 2} } }
set queryInfo { { {Title {1=4 4=1}} {Author {1=1}} \
        {Subject {1=21}} {Any {1=1016}} } }
wm minsize . 0 0

set setOffset 0
set setMax 0

# Procedure tkerror {err}
#   err   error message
# Override the Tk error handler function.
proc tkerror err {
    set w .tkerrorw

    if {[winfo exists $w]} {
        destroy $w
    }
    toplevel $w
    wm title $w "Error"

    place-force $w .
    top-down-window $w

    label $w.top.b -bitmap error
    message $w.top.t -aspect 300 -text "Error: $err" \
            -font -Adobe-Helvetica-Bold-R-Normal-*-180-*
    pack $w.top.b $w.top.t -side left -padx 10 -pady 10

    bottom-buttons $w [list {Close} [list destroy $w]] 1
}

# Read tag set file (if present)
if {[file readable "${libdir}/tagsets.tcl"]} {
    source "${libdir}/tagsets.tcl"
}

# Read the global configuration file.
if {[file readable "clientrc.tcl"]} {
    source "clientrc.tcl"
} elseif {[file readable "${libdir}/clientrc.tcl"]} {
    source "${libdir}/clientrc.tcl"
}

# Make old definitions up-to-date.
foreach n [array names profile] {
    set l [llength $profile($n)]
    while {$l < 29} {
        lappend profile($n) {}
        incr l
    }
}

# Read the user configuration file.
if {[file readable "~/.clientrc.tcl"]} {
    source "~/.clientrc.tcl"
}

# These globals describe the current query type. They are set to the
# first query type.
set queryButtonsFind [lindex $queryButtons 0]
set queryInfoFind [lindex $queryInfo 0]

# Procedure read-formats
# Read all Tcl source files in the subdirectory 'formats'.
# The name of each source will correspond to a display format.
proc read-formats {} {
    global displayFormats
    global libdir

    set oldDir [pwd]
    cd ${libdir}/formats
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
    if {$debugMode} {
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
		-relief flat -borderwidth 0 -yscrollcommand [list $w.top.s set]
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
    global displayFormat
    global setNo
    global busy

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

# Procedure post-menu {wbutton wmenu}
#   wbutton    button widget
#   wmenu      menu widget
# Post menu near button. Note: not used.
proc post-menu {wbutton wmenu} {
    $wmenu activate none
    focus $wmenu
    $wmenu post [winfo rootx $wbutton] \
            [expr [winfo rooty $wbutton]+[winfo height $wbutton]]

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

    toplevel $w
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
    button $w.bot.left.ok -width 4 -text {Ok} \
            -command ${ok-action}
    pack $w.bot.left.ok -expand yes -ipadx 1 -ipady 1 -padx 2 -pady 2
    button $w.bot.cancel -width 5 -text {Cancel} \
            -command [list destroy $w]
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
            -command [lindex $buttonList [expr $i+1]]
    pack $w.bot.$i.ok -expand yes -padx 2 -pady 2 -side left

    incr i 2
    while {$i < $l} {
        button $w.bot.$i -text [lindex $buttonList $i] \
                -command [lindex $buttonList [expr $i+1]]
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
# when this procedure exists.
proc cancel-operation {} {
    global cancelFlag
    global busy
    global delayRequest

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
    global profile

    if {![string length $target]} {
        .bot.a.target configure -text ""
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
    global busy
    global libdir

    if {$busy != 0} {
        incr v1
        if {$v1==10} {
            set v1 1
        }
        .bot.logo configure -bitmap @${libdir}/bitmaps/book${v1}
        after 140 [list show-logo $v1]
        return
    }
    while {1} {
        .bot.logo configure -bitmap @${libdir}/bitmaps/book1
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
    global busy
    global scanEnable
    global setOffset
    global setMax
    global setNo

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
    set start [$w index insert]
    $w insert insert $text
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start insert
    }
    foreach i $args {
        $w tag add $i $start insert
    }
}

# Procedure popup-license
# Displays LICENSE information.
proc popup-license {} {
    global libdir
    set w .popup-licence
    toplevel $w

    wm title $w "License" 

    wm minsize $w 0 0

    top-down-window $w

    text $w.top.t -width 80 -height 10 -wrap word -relief flat -borderwidth 0 \
        -font fixed -yscrollcommand [list $w.top.s set]
    scrollbar $w.top.s -command [list $w.top.t yview]
    
    pack $w.top.s -side right -fill y
    pack $w.top.t -expand yes -fill both

    if {[file readable "${libdir}/LICENSE"]} {
        set f [open "${libdir}/LICENSE" r]
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
    global hostid

    toplevel $w

    wm title $w "About target"
    place-force $w .
    top-down-window $w

    frame $w.top.a -relief ridge -border 2
    frame $w.top.p -relief ridge -border 2

    pack $w.top.a $w.top.p -side top -fill x
    
    label $w.top.a.about -text "About"
    label $w.top.a.irtcl -text $hostid \
            -font -Adobe-Helvetica-Bold-R-Normal-*-240-*
    pack $w.top.a.about $w.top.a.irtcl -side top

    set i [z39 targetImplementationName]
    label $w.top.p.in -text "Implementation name: $i"
    set i [z39 targetImplementationId]
    label $w.top.p.ii -text "Implementation id: $i"
    set i [z39 targetImplementationVersion]
    label $w.top.p.iv -text "Implementation version: $i"
    set i [z39 options]
    label $w.top.p.op -text "Protocol options: $i"

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
    $w.top.a.logo configure -bitmap @${libdir}/bitmaps/book$n
    after 140 [list about-origin-logo $n]
}

# Procedure about-origin
# Display various information about origin (this client).
proc about-origin {} {
    set w .about-origin-w
    global libdir
    global tk_version
    
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
    
    label $w.top.a.irtcl -text "IrTcl" \
            -font -Adobe-Helvetica-Bold-R-Normal-*-240-*
    label $w.top.a.logo -bitmap @${libdir}/bitmaps/book1 
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
    global displayFormats
    global popupMarcdf

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
                -borderwidth 0 -font fixed -yscrollcommand [list $w.top.s set]
        scrollbar $w.top.s -command [list $w.top.record yview]

        global monoFlag
        if {! $monoFlag} {
            $w.top.record tag configure marc-tag -foreground blue
            $w.top.record tag configure marc-id -foreground red
        } else {
            $w.top.record tag configure marc-tag -foreground black
            $w.top.record tag configure marc-id -foreground black
        }
        $w.top.record tag configure marc-data -foreground black
        $w.top.record tag configure marc-head \
                -font -Adobe-Times-Medium-R-Normal-*-180-* \
                -background black -foreground white

        $w.top.record tag configure marc-pref \
                -font -Adobe-Times-Medium-R-Normal-*-180-* \
                -foreground blue
        $w.top.record tag configure marc-text \
                -font -Adobe-Times-Medium-R-Normal-*-180-* \
                -foreground black
        $w.top.record tag configure marc-it \
                -font -Adobe-Times-Medium-I-Normal-*-180-* \
                -foreground black

        pack $w.top.s -side right -fill y
        pack $w.top.record -expand yes -fill both
        
        bottom-buttons $w [list \
                {Close} [list destroy $w] \
                {Prev} {} \
                {Next} {} \
                {Duplicate} {}] 0
        menubutton $w.bot.formats -text "Format" -menu $w.bot.formats.m \
                -relief raised
        menu $w.bot.formats.m
        pack $w.bot.formats -expand yes -ipadx 2 -ipady 2 \
                -padx 3 -pady 3 -side left
    } else {
        $w.bot.formats.m delete 0 last
    }
    set i 0
    foreach f $displayFormats {
        $w.bot.formats.m add radiobutton -label $f \
                -variable popupMarcdf -value $i \
                -command [list popup-marc $sno $no $b 0]
        incr i
    }
    $w.top.record delete 0.0 end
    set recordType [z39.$sno recordType $no]
    wm title $w "$recordType record #$no"

    $w.bot.2 configure -command \
            [list popup-marc $sno [expr $no-1] $b $df]
    $w.bot.4 configure -command \
            [list popup-marc $sno [expr $no+1] $b $df]
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
        if {[tk4]} {
            .top.target.m delete 7 [expr 7+$olen]
        } else {
            .top.target.m delete 6 [expr 6+$olen]
        }
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
    foreach n [array names profile] {
        if {$n == $target} {
            destroy .target-define
            protocol-setup $n
            return
        }
    }
    set seq [lindex $profile(Default) 12]
    dputs "seq=${seq}"
    dputs $profile(Default)
    set profile($target) $profile(Default)
    set profile(Default) [lreplace $profile(Default) 12 12 [incr seq]]
   
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
    tkerror "$m ($c)"
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
    global profile
    global hostid
    global presentChunk

    set desc [lindex $profile($target) 0]
    if {[string length $desc]} {
        .data.record insert end $desc
    } else {
        .data.record insert end $target
    }
    .data.record insert end "\n\n"

    z39 disconnect
    z39 comstack [lindex $profile($target) 6]
    z39 protocol [lindex $profile($target) 11]
    eval z39 idAuthentication [lindex $profile($target) 3]
    z39 maximumRecordSize [lindex $profile($target) 4]
    z39 preferredMessageSize [lindex $profile($target) 5]
    dputs "maximumRecordSize=[z39 maximumRecordSize]"
    dputs "preferredMessageSize=[z39 preferredMessageSize]"
    show-status Connecting 1 0
    set x [lindex $profile($target) 13]
    if {![string length $x]} {
        set x 2
    }
    z39 largeSetLowerBound $x
    
    set x [lindex $profile($target) 14]
    if {![string length $x]} {
        set x 0
    }
    z39 smallSetUpperBound $x
    
    set x [lindex $profile($target) 15]
    if {![string length $x]} {
        set x 0
    }
    z39 mediumSetPresentNumber $x

    set presentChunk [lindex $profile($target) 16]
    if {![string length $presentChunk]} {
        set presentChunk 4
    }

    z39 failback [list fail-response $target]
    z39 callback [list connect-response $target $base]
    show-target $target $base
    update idletasks
    set err [catch {
        z39 connect [lindex $profile($target) 1]:[lindex $profile($target) 2]
        } errorMessage]
    if {$err} {
        set hostid Default
        tkerror $errorMessage
        show-status "Not connected" 0 {}
        show-target {} {}
        return
    }
    set hostid $target
    configure-disable-e .top.target.m 0
    configure-enable-e .top.target.m 1
    configure-enable-e .top.target.m 2
}

# Procedure close-target
# Shuts down the connection with current target.
proc close-target {} {
    global hostid
    global cancelFlag
    global setNo
    global setNoLast

    set cancelFlag 0
    set setNo 0
    set setNoLast 0
    .bot.a.set configure -text ""
    set hostid Default
    z39 disconnect
    show-target {} {}
    show-status {Not connected} 0 0
    init-title-lines
    show-message {}
    configure-disable-e .top.target.m 1
    configure-disable-e .top.target.m 2
    if {[tk4]} {
        .top.rset.m delete 2 last
    } else {
        .top.rset.m delete 1 last
    }
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

    frame $w.top.filename
    pack $w.top.filename -side top -anchor e -pady 2
    
    entry-fields $w.top {filename} \
            {{Filename:}} \
            {load-set-action} {destroy .load-set}
    
    top-down-ok-cancel $w {load-set-action} 1
    focus $oldFocus
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
        tkerror $errorMessage
        show-status Ready 0 {}
    }
}

# Procedure init-response
# Handles and incoming init-response. The service buttons
# are enabled. The global $scanEnable indicates whether the target
# supports scan.
proc init-response {target base} {
    global cancelFlag profile
    global scanEnable settingsChanged

    dputs {init-response}
    apduDump
    if {$cancelFlag} {
        close-target
        return
    }
    if {![z39 initResult]} {
        set u [z39 userInformationField]
        close-target
        tkerror "Connection rejected by target: $u"
    } else {
        explain-check $target [list ready-response $base]
    }
}

# Procedure explain-check 
# Stub function to check explain. May be overwritten later.
proc explain-check {target response} {
    eval $response [list $target]
}

# Procedure ready-response
# Called after a target has been initialized and, possibly, explained
proc ready-response {base target} {
    global profile settingsChanged scanEnable
    
    if {![string length $base]} {
        set base [lindex [lindex $profile($target) 7] 0]
    }
    if {![string length $base]} {
        set base Default
    }
    z39 databaseNames $base
    set profile($target) [lreplace $profile($target) 18 18 [clock seconds]]
    set settingsChanged 1
    if {[lsearch [z39 options] scan] >= 0} {
        set scanEnable 1
    } else {
        set scanEnable 0
    }
    cascade-dblist $target $base
    show-target $target $base
    show-message {}
    show-status Ready 0 1

    .data.record insert end [lindex $profile($target) 27]
    .data.record insert end "\n"
    set data [lindex $profile($target) 21]
    if {[string length $data]} {
        .data.record insert end "News:\n"
        .data.record insert end "$data\n"
    }
}

# Procedure search-request
#  bflag     flag to indicate if this procedure calls itself
# Performs a search. If $busy is 1, the search-request is performed
# at a later time (when another response arrives). This procedure
# sets many search-related Z39-settings. The global $setNo is set
# to the result set number (z39.$setNo).
proc search-request {bflag} {
    global setNo
    global setNoLast
    global profile
    global hostid
    global busy
    global cancelFlag
    global delayRequest
    global recordSyntax
    global elementSetNames

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
    if {![string length $query]} {
        return
    }
    incr setNoLast
    set setNo $setNoLast
    ir-set z39.$setNo z39

    if {[lindex $profile($target) 10] == 1} {
        z39.$setNo setName $setNo
        dputs "setName=${setNo}"
    } else {
        z39.$setNo setName Default
        dputs "setName=Default"
    }
    if {[lindex $profile($target) 8] == 1} {
        z39.$setNo queryType rpn
    }
    if {[lindex $profile($target) 9] == 1} {
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
    z39.$setNo search $query
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

    global profile
    global hostid
    global scanView
    global scanTerm
    global curIndexEntry
    global queryButtonsFind
    global queryInfoFind
    global cancelFlag
    global delayRequest

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
                -font fixed 
        scrollbar $w.top.scroll -orient vertical -border 1
        pack $w.top.list -side left -fill both -expand yes
        pack $w.top.scroll -side right -fill y
        $w.top.scroll config -command [list $w.top.list yview]
        
        bottom-buttons $w [list {Close} [list destroy $w] \
                {Up} [list scan-up $attr] \
                {Down} [list scan-down $attr]] 0
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
    global cancelFlag
    global delayRequest
    global scanTerm
    global scanView

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
        tkerror "Scan fail"
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
    global scanView
    global cancelFlag
    global delayRequest

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
    global scanView
    global cancelFlag
    global delayRequest

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
    global setNo
    global setOffset
    global setMax
    global cancelFlag
    global busy
    global delayRequest
    global presentChunk

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
        tkerror "NSD$code: $msg: $addinfo"
	dputs "xxxxxxxxxxxxxxx"
        return
    }
    show-message "${setMax} hits"
    if {$setMax == 0} {
        return
    }
    set setOffset 1
    show-status Ready 0 1
    set l [format "%-4d %7d" $setNo $setMax]
    .top.rset.m add command -label $l \
            -command [list recall-set $setNo]
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
    global setNo
    global setOffset
    global setMax
    global busy
    global cancelFlag
    global delayRequest
    global presentChunk

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
    .data.record delete 0.0 end
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
    global displayFormats
    global displayFormat
    global setNo
    global busy

    dputs "add-title-lines offset=${offset} no=${no}"
    if {$setno != -1} {
        set setNo $setno
    } else {
        set setno $setNo
    }
    if {$offset == 1} {
        .bot.a.set configure -text $setno
        .data.record delete 0.0 end
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
        .data.record tag bind r$o <1> \
                [list popup-marc $setno $o 0 0]
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
    global setNo
    global setOffset
    global setMax
    global cancelFlag
    global delayRequest
    global presentChunk

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
        tkerror "NSD$code: $msg: $addinfo"
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
        if {![tk4]} {
            bind [lindex $list $i] <Tab> \
                    [list focus [lindex $list [expr $i+1]]]
            bind [lindex $list $i] <Left> \
                    [list left-cursor [lindex $list $i]]
            bind [lindex $list $i] <Right> \
                    [list right-cursor [lindex $list $i]]
        }
    }
    bind [lindex $list $i] <Return> $returnAction
    bind [lindex $list $i] <Escape> $escapeAction
    if {![tk4]} {
        bind [lindex $list $i] <Tab>  [list focus [lindex $list 0]]
        bind [lindex $list $i] <Left> [list left-cursor [lindex $list $i]]
        bind [lindex $list $i] <Right> [list right-cursor [lindex $list $i]]
    }
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
    pack $w.top.target \
            -side top -anchor e -pady 2 
    entry-fields $w.top {target} \
            {{Target:}} \
            {define-target-action} {destroy .target-define}
    top-down-ok-cancel $w {define-target-action} 1
}

# Procedure protocol-setup-delete
# This procedure is invoked when the user tries to delete a target
# definition. If user is sure, the target definition is deleted.
proc protocol-setup-delete {target w} {
    global profile
    global settingsChanged

    set a [alert "Are you sure you want to delete the target \
definition $target ?"]
    if {$a} {
        destroy $w
        unset profile($target)
        set settingsChanged 1
        cascade-target-list
        delete-target-hotlist $target
    }
}

# Procedure protocol-setup-action {target w}
# target     target to be defined
# w          target definition toplevel widget
# This procedure reads all appropriate globals and makes a new/modified
# profile for the target. The global array $targetS contains most of the
# information the user may modify.
proc protocol-setup-action {target w} {
    global profile
    global settingsChanged
    global targetS

    set dataBases {}
    set settingsChanged 1
    set len [$w.top.databases.list size]
    for {set i 0} {$i < $len} {incr i} {
        lappend dataBases [$w.top.databases.list get $i]
    }
    set wno [lindex $profile($target) 12]
    set timedef [lindex $profile($target) 17]
    if {![string length $timedef]} {
        set timedef [clock seconds]
    }

    set idauth [$w.top.idAuthentication.entry get]

    set profile($target) [list [$w.top.description.entry get] \
            [$w.top.host.entry get] \
            [$w.top.port.entry get] \
            $idauth \
            $targetS($target,MRS) \
            $targetS($target,PMS) \
            $targetS($target,csType) \
            $dataBases \
            $targetS($target,RPN) \
            $targetS($target,CCL) \
            $targetS($target,ResultSets) \
            $targetS($target,protocolType) \
            $wno \
            $targetS($target,LSLB) \
            $targetS($target,SSUB) \
            $targetS($target,MSPN) \
            $targetS($target,presentChunk) \
            $timedef \
            {} \
            {} ]

    cascade-target-list
    delete-target-hotlist $target
    dputs $profile($target)
    destroy $w
}

# Procedure place-force {window parent}
#  window      new top level widget
#  parent      parent widget used as base
# Sets geometry of $window relative to $parent window.
proc place-force {window parent} {
    set g [wm geometry $parent]

    set p1 [string first + $g]
    set p2 [string last + $g]

    set x [expr 40+[string range $g [expr $p1 +1] [expr $p2 -1]]]
    set y [expr 60+[string range $g [expr $p2 +1] end]]
    wm geometry $window +${x}+${y}
}

# Procedure add-database-action {target w}
#  target      target to be defined
#  w           top level widget for the target definition
# Adds the contents of .database-select.top.database.entry to list of
# databases.
proc add-database-action {target w} {
    global profile

    $w.top.databases.list insert end \
            [.database-select.top.database.entry get]
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
    
    entry-fields $w.top {database} \
            {{Database to add:}} \
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

# Procedure protocol-setup {target}
#  target     target to be defined
# Makes a dialog in which the user may modify/view a target definition
# (profile). The $targetS - array holds the initial definition of the
# target.
proc protocol-setup {target} {
    global profile
    global targetS
    
    set bno 0
    while {[winfo exists .setup-$bno]} {
        incr bno
    }
    set w .setup-$bno

    toplevelG $w

    wm title $w "Setup $target"

    top-down-window $w
    
    if {![string length $target]} {
        set target Default
    }
    dputs target
    dputs $profile($target)

    frame $w.top.description
    frame $w.top.host
    frame $w.top.port
    frame $w.top.idAuthentication

    frame $w.top.cs-type -relief ridge -border 2
    frame $w.top.protocol -relief ridge -border 2
    frame $w.top.query -relief ridge -border 2
    frame $w.top.databases -relief ridge -border 2

    # Maximum/preferred/idAuth ...
    pack $w.top.description $w.top.host $w.top.port \
            $w.top.idAuthentication -side top -anchor e -pady 2
    
    entry-fields $w.top {description host port idAuthentication } \
            {{Description:} {Host:} {Port:} {Id Authentication:}} \
            [list protocol-setup-action $target $w] [list destroy $w]
    
    foreach sub {description host port idAuthentication} {
        dputs $sub
        bind $w.top.$sub.entry <Control-a> [list add-database $target $w]
        bind $w.top.$sub.entry <Control-d> [list delete-database $target $w]
    }
    $w.top.description.entry insert 0 [lindex $profile($target) 0]
    $w.top.host.entry insert 0 [lindex $profile($target) 1]
    $w.top.port.entry insert 0 [lindex $profile($target) 2]
    $w.top.idAuthentication.entry insert 0 [lindex $profile($target) 3]
    set targetS($target,csType) [lindex $profile($target) 6]
    set targetS($target,RPN) [lindex $profile($target) 8]
    set targetS($target,CCL) [lindex $profile($target) 9]
    set targetS($target,ResultSets) [lindex $profile($target) 10]
    set targetS($target,protocolType) [lindex $profile($target) 11]
    if {![string length $targetS($target,protocolType)]} {
        set targetS($target,protocolType) Z39
    }
    set targetS($target,LSLB) [lindex $profile($target) 13]
    set targetS($target,SSUB) [lindex $profile($target) 14]
    set targetS($target,MSPN) [lindex $profile($target) 15]
    set targetS($target,presentChunk) [lindex $profile($target) 16]
    set targetS($target,MRS) [lindex $profile($target) 4]
    set targetS($target,PMS) [lindex $profile($target) 5]

    # Databases ....
    pack $w.top.databases -side left -pady 2 -padx 2 -expand yes -fill both

    label $w.top.databases.label -text "Databases"
    button $w.top.databases.add -text Add \
            -command [list add-database $target $w]
    button $w.top.databases.delete -text Delete \
            -command [list delete-database $target $w]
    if {! [tk4]} {
        listbox $w.top.databases.list -geometry 14x6 \
                -yscrollcommand "$w.top.databases.scroll set"
    } else {
        listbox $w.top.databases.list -width 14 -height 5\
                -yscrollcommand "$w.top.databases.scroll set"
    }
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
    pack $w.top.cs-type -pady 2 -padx 2 -side top -fill x
    
    label $w.top.cs-type.label -text "Transport" 
    radiobutton $w.top.cs-type.tcpip -text "TCP/IP" -anchor w \
            -variable targetS($target,csType) -value tcpip
    radiobutton $w.top.cs-type.mosi -text "MOSI" -anchor w\
            -variable targetS($target,csType) -value mosi
    
    pack $w.top.cs-type.label $w.top.cs-type.tcpip $w.top.cs-type.mosi \
            -padx 2 -side top -fill x

    # Protocol ...
    pack $w.top.protocol -pady 2 -padx 2 -side top -fill x
    
    label $w.top.protocol.label -text "Protocol" 
    radiobutton $w.top.protocol.z39v2 -text "Z39.50" -anchor w \
            -variable targetS($target,protocolType) -value Z39
    radiobutton $w.top.protocol.sr -text "SR" -anchor w \
            -variable targetS($target,protocolType) -value SR
    
    pack $w.top.protocol.label $w.top.protocol.z39v2 $w.top.protocol.sr \
            -padx 2 -side top -fill x

    # Query ...
    pack $w.top.query -pady 2 -padx 2 -side top -fill x

    label $w.top.query.label -text "Query support"
    checkbutton $w.top.query.c1 -text "RPN query" -anchor w \
            -variable targetS($target,RPN)
    checkbutton $w.top.query.c2 -text "CCL query" -anchor w \
            -variable targetS($target,CCL)
    checkbutton $w.top.query.c3 -text "Result sets" -anchor w \
            -variable targetS($target,ResultSets)

    pack $w.top.query.label -side top 
    pack $w.top.query.c1 $w.top.query.c2 $w.top.query.c3 \
            -padx 2 -side top -fill x

    # Ok-cancel
    bottom-buttons $w [list {Ok} [list protocol-setup-action $target $w] \
            {Delete} [list protocol-setup-delete $target $w] \
            {Advanced} [list advanced-setup $target $bno] \
            {Cancel} [list destroy $w]] 0   
}

# Procedure advanced-setup {target b}
#  target     target to be defined
#  b          window number of target top level
# Makes a dialog in which the user may modify/view advanced settings
# of a target definition (profile).
proc advanced-setup {target b} {
    global profile
    global targetS

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

    $w.top.largeSetLowerBound.entry insert 0 $targetS($target,LSLB)
    $w.top.smallSetUpperBound.entry insert 0 $targetS($target,SSUB)
    $w.top.mediumSetPresentNumber.entry insert 0 $targetS($target,MSPN)
    $w.top.presentChunk.entry insert 0 $targetS($target,presentChunk)
    $w.top.maximumRecordSize.entry insert 0 $targetS($target,MRS)
    $w.top.preferredMessageSize.entry insert 0 $targetS($target,PMS)
    
    bottom-buttons $w [list {Ok} [list advanced-setup-action $target $b] \
            {Cancel} [list destroy $w]] 0   
}

# Procedure advanced-setup-action {target b}
#  target     target to be defined
#  b          window number of target top level
# This procedure is called when the user hits Ok in the advanced target
# setup dialog. The temporary result is stored in the $targetS - array.
proc advanced-setup-action {target b} {
    set w .advanced-setup-$b
    global targetS
    
    set targetS($target,LSLB) [$w.top.largeSetLowerBound.entry get]
    set targetS($target,SSUB) [$w.top.smallSetUpperBound.entry get]
    set targetS($target,MSPN) [$w.top.mediumSetPresentNumber.entry get]
    set targetS($target,presentChunk) [$w.top.presentChunk.entry get]
    set targetS($target,MRS) [$w.top.maximumRecordSize.entry get]
    set targetS($target,PMS) [$w.top.preferredMessageSize.entry get]

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
    global profile
    global hostid

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
    focus $oldFocus
}

# Procedure cascase-dblist-select
proc cascade-dblist-select {target db} {
    show-target $target $db
    z39 databaseNames $db
}

# Procedure cascade-dblist 
# Makes the Service/database list with proper databases for the target
proc cascade-dblist {target base} {
    global profile

    set w .top.service.m.dblist
    $w delete 0 200
    foreach db [lindex $profile($target) 7] {
        $w add command -label $db \
                -command [list cascade-dblist-select $target $db]
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
    foreach n [lsort [array names profile]] {
        if {$n != "Default"} {
            set nl [lindex $profile($n) 12]
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
    foreach n [lsort [array names profile]] {
        .top.target.m.slist add command -label $n \
                -command [list protocol-setup $n]
    }
}

# Procedure query-select {i}
#  i       Query type number (integer)
# This procedure is called when the user selects a Query type. The current
# query type information given by the globals $queryButtonsFind and
# $queryInfoFind are affected by this operation.
proc query-select {i} {
    global queryButtonsFind
    global queryInfoFind
    global queryButtons
    global queryInfo

    set queryInfoFind [lindex $queryInfo $i]
    set queryButtonsFind [lindex $queryButtons $i]

    index-lines .lines 1 $queryButtonsFind $queryInfoFind activate-index
}

# Procedure query-new-action 
# Commits a new query type definition by extending the globals
# $queryTypes, $queryButtons and $queryInfo.
proc query-new-action {} {
    global queryTypes
    global queryButtons
    global queryInfo
    global settingsChanged

    set settingsChanged 1
    lappend queryTypes [.query-new.top.index.entry get]
    lappend queryButtons {}
    lappend queryInfo {}

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
    pack $w.top.index \
            -side top -anchor e -pady 2 
    entry-fields $w.top index \
            {{Query Name:}} \
            query-new-action {destroy .query-new}
    top-down-ok-cancel $w query-new-action 1
    focus $oldFocus
}

# Procedure query-delete-action {queryNo}
#  queryNo     query type number (integer)
# Procedure that deletes the query type specified by $queryNo.
proc query-delete-action {queryNo} {
    global queryTypes
    global queryButtons
    global queryInfo
    global settingsChanged

    set settingsChanged 1

    set queryTypes [lreplace $queryTypes $queryNo $queryNo]
    set queryButtons [lreplace $queryButtons $queryNo $queryNo]
    set queryInfo [lreplace $queryInfo $queryNo $queryNo]
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
# Updates the enties below Options|Query to list all query types.
proc cascade-query-list {} {
    global queryTypes
    set w .top.options.m.query

    set i 0
    $w.slist delete 0 last
    foreach n $queryTypes {
        $w.slist add command -label $n -command [list query-setup $i]
        incr i
    }

    set i 0
    $w.clist delete 0 last
    foreach n $queryTypes {
        $w.clist add command -label $n -command [list query-select $i]
        incr i
    }
    set i 0
    $w.dlist delete 0 last
    foreach n $queryTypes {
        $w.dlist add command -label $n -command [list query-delete $i]
        incr i
    }
}

# Procedure save-geometry
# This procedure saves the per-user related settings in ~/.clientrc.tcl.
# The geometry information stored in the global array $windowGeometry is
# saved. Also a few other user settings, such as current display format, are
# saved.
proc save-geometry {} {
    global windowGeometry
    global hotTargets
    global textWrap
    global displayFormat
    global popupMarcdf
    global recordSyntax
    global elementSetNames
    global hostid

    set windowGeometry(.) [wm geometry .]

    if {[catch {set f [open ~/.clientrc.tcl w]}]} {
        return
    } 
    if {$hostid != "Default"} {
        puts $f "set hostid \{$hostid\}"
        set b [z39 databaseNames]
        puts $f "set hostbase $b"
    }
    puts $f "set hotTargets \{ $hotTargets \}"
    puts $f "set textWrap $textWrap"
    puts $f "set displayFormat $displayFormat"
    puts $f "set popupMarcdf $popupMarcdf"
    puts $f "set recordSyntax $recordSyntax"
    puts $f "set elementSetNames $elementSetNames"
    foreach n [array names windowGeometry] {
        puts -nonewline $f "set \{windowGeometry($n)\} \{"
        puts -nonewline $f $windowGeometry($n)
        puts $f "\}"
    }
    close $f
}

# Procedure save-settings
# This procedure saves the per-host related settings clientrc.tcl which
# is normally kept in the directory /usr/local/lib/irtcl.
# All query types and target defintion profiles are saved.
proc save-settings {} {
    global profile
    global libdir
    global settingsChanged
    global queryTypes
    global queryButtons
    global queryInfo

    if {[file exists clientrc.tcl]} {
        set f [open "clientrc.tcl" w]
    } elseif {![file writable "${libdir}/clientrc.tcl"]} {
        set a [alert "Cannot open ${libdir}/clientrc.tcl for writing. Do you \
                wish to save clientrc.tcl in the current directory instead?"]
        if {! $a} {
            return
        }
        set f [open "clientrc.tcl" w]
    } else {
        set f [open "${libdir}/clientrc.tcl" w]
    }
    puts $f "# Setup file"

    foreach n [array names profile] {

        puts -nonewline $f "set \{profile($n)\} \{"
        puts -nonewline $f $profile($n)
        puts $f "\}"
        puts $f {}
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

# Procedure alert {ask}
#  ask    prompt string
# Makes a grabbed dialog in which the user is requested to answer
# "Ok" or "Cancel". This procedure returns 1 if the user hits "Ok"; 0
# otherwise.
proc alert {ask} {
    set w .alert

    global alertAnswer

    toplevel $w
    set oldFocus [focus]
    place-force $w .
    top-down-window $w

    label $w.top.warning -bitmap warning
    message $w.top.message -text $ask -aspect 300 \
            -font -Adobe-Times-Medium-R-Normal-*-180-*

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
    if {[winfo exists $button]} {
        $button configure -text [lindex [lindex $names $no] 0]
        ${button}.m delete 0 last
    } else {
        menubutton $button -text [lindex [lindex $names $no] 0] \
                -width 10 -menu ${button}.m -relief raised -border 1
        menu ${button}.m
        if {[tk4]} {
            ${button}.m configure -tearoff off
	}
    }
    set i 0
    foreach name $names {
        ${button}.m add command -label [lindex $name 0] \
                -command [list listbuttonaction ${button} $name \
                $handle $user $i]
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
    menu ${button}.m
    if {[tk4]} {
        ${button}.m configure -tearoff off
    }
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

    set $var [lindex $names [expr $i+1]]
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
    eval "set val $$var"
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
    menubutton $button -text $n -menu ${button}.m \
            -relief raised -border 1
    menu ${button}.m
    if {[tk4]} {
        ${button}.m configure -tearoff off
    }
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

    global queryInfoTmp
    global queryButtonsTmp

    set newI [.query-add-index.top.index.entry get]
    lappend queryInfoTmp [list $newI {}]
    $w.top.index.list insert end $newI
    destroy .query-add-index
    #destroy $w.top.lines
    #frame $w.top.lines -relief ridge -border 2
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
    #pack $w.top.lines -side left -pady 6 -padx 6 -fill y
}

# Procedure query-add-line
#  queryNo      query type number (integer)
# Handler that adds new query line.
proc query-add-line {queryNo} {
    set w .query-setup

    global queryInfoTmp
    global queryButtonsTmp

    lappend queryButtonsTmp {I 0}

    #destroy $w.top.lines
    #frame $w.top.lines -relief ridge -border 2
    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index
    #pack $w.top.lines -side left -pady 6 -padx 6 -fill y
}

# Procedure query-del-line
#  queryNo      query type number (integer)
# Handler that removes query line.
proc query-del-line {queryNo} {
    set w .query-setup

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
    pack $w.top.index \
            -side top -anchor e -pady 2 
    entry-fields $w.top {index} \
            {{Index Name:}} \
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
    global queryButtons
    global queryInfo
    global queryButtonsTmp
    global queryInfoTmp
    global queryButtonsFind
    global queryInfoFind
    
    global settingsChanged 

    set settingsChanged 1

    set queryInfo [lreplace $queryInfo $queryNo $queryNo \
            $queryInfoTmp]
    set queryButtons [lreplace $queryButtons $queryNo $queryNo \
            $queryButtonsTmp]
    set queryInfoFind $queryInfoTmp
    set queryButtonsFind $queryButtonsTmp

    destroy .query-setup
    index-lines .lines 1 $queryButtonsFind $queryInfoFind activate-index
}

# Procedure activate-e-index {value no i}
#   value   menu name
#   no      query index number
#   i       menu index (integer)
# Procedure called when listbutton is activated in the query type edit
# window. The global $queryButtonsTmp is updated in this operation.
proc activate-e-index {value no i} {
    global queryButtonsTmp
    global queryIndexTmp
    
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
    set attr {
        {None}                           0
        {Personal name}                  1 
        {Corporate name}                 2 
        {Conference name}                3 
        {Title}                          4 
        {Title-series}                   5 
        {Title-uniform}                  6 
        {ISBN}                           7 
        {ISSN}                           8 
        {LC card number}                 9 
        {BNB card number}                10
        {BGF(sic) number}                11 
        {Local number}                   12 
        {Dewey classification}           13 
        {UDC classification}             14 
        {Bliss classification}           15 
        {LC call number}                 16 
        {NLM call number}                17 
        {NAL call number}                18 
        {MOS call number}                19 
        {Local classification}           20 
        {Subject heading}                21 
        {Subject-RAMEAU}                 22 
        {BDI-index-subject}              23 
        {INSPEC-subject}                 24 
        {MESH-subject}                   25 
        {PA-subject}                     26 
        {LC-subject-heading}             27 
        {RVM-subject-heading}            28 
        {Local subject index}            29 
        {Date}                           30 
        {Date of publication}            31 
        {Date of acquisition}            32 
        {Title-key}                      33 
        {Title-collective}               34 
        {Title-parallel}                 35 
        {Title-cover}                    36 
        {Title-added-title-page}         37 
        {Title-caption}                  38 
        {Title-running}                  39 
        {Title-spine}                    40 
        {Title-other-variant}            41 
        {Title-former}                   42 
        {Title-abbreviated}              43 
        {Title-expanded}                 44 
        {Subject-PRECIS}                 45 
        {Subject-RSWK}                   46 
        {Subject-subdivision}            47 
        {Number-natl-bibliography}       48 
        {Number-legal-deposit}           49 
        {Number-govt-publication}        50 
        {Number-publisher-for-music}     51 
        {Number-DB}                      52 
        {Number-local-call}              53 
        {Code-language}                  54 
        {Code-geographic-area}           55 
        {Code-institution}               56 
        {Name and title}                 57 
        {Name-geographic}                58 
        {Place-publication}              59 
        {CODEN}                          60 
        {Microform-generation}           61 
        {Abstract}                       62 
        {Note}                           63 
        {Author-title}                 1000 
        {Record type}                  1001 
        {Name}                         1002 
        {Author}                       1003 
        {Author-name-personal}         1004 
        {Author-name-corporate}        1005 
        {Author-name-conference}       1006 
        {Identifier-standard}          1007 
        {Subject-LC-children's}        1008 
        {Subject-name-personal}        1009 
        {Body of text}                 1010 
        {Date/time added to database}  1011 
        {Date/time last modified}      1012 
        {Authority/format identifier}  1013 
        {Concept-text}                 1014 
        {Concept-reference}            1015 
        {Any}                          1016 
        {Server choice}                1017 
        {Publisher}                    1018 
        {Record source}                1019 
        {Editor}                       1020 
        {Bib-level}                    1021 
        {Geographic class}             1022 
        {Indexed by}                   1023 
        {Map scale}                    1024 
        {Music key}                    1025 
        {Related periodical}           1026 
        {Report number}                1027 
        {Stock number}                 1028 
        {Thematic number}              1030 
        {Material type}                1031 
        {Doc ID}                       1032 
        {Host item}                    1033 
        {Content type}                 1034 
        {Anywhere}                     1035 
    }
    set w .index-setup
    global useTmpValue
    set l [llength $attr]

    if {$init} {
        set s 0
        set lno 0
        for {set i 0} {$i < $l} {incr i} {
            $w.top.use.list insert end [lindex $attr $i]
            incr i
            if {$useTmpValue == [lindex $attr $i]} {
                set s $lno
            }
            incr lno
        }
        if {[tk4]} {
            $w.top.use.list selection clear 0 end
            $w.top.use.list selection set $s $s
        } else {
            $w.top.use.list select from $s
            $w.top.use.list select to $s
        }
        incr s -3
        if {$s < 0} {
            set s 0
        }
        $w.top.use.list yview $s
    } else {
        set lno [lindex [$w.top.use.list curselection] 0]
        set i [expr $lno+$lno+1]
        set useTmpValue [lindex $attr $i]
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

    global useTmpValue
    global relationTmpValue
    global structureTmpValue
    global truncationTmpValue
    global completenessTmpValue
    global positionTmpValue
    global queryInfoTmp

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

    global relationTmpValue
    global structureTmpValue
    global truncationTmpValue
    global completenessTmpValue
    global positionTmpValue
    global useTmpValue
    set relationTmpValue 0
    set truncationTmpValue 0
    set structureTmpValue 0
    set positionTmpValue 0
    set completenessTmpValue 0
    set useTmpValue 0

    catch {destroy $w}
    toplevelG $w

    set n [lindex $attr 0]
    wm title $w "Index setup $n"

    top-down-window $w

    set len [llength $attr]
    for {set i 1} {$i < $len} {incr i} {
        set q [lindex $attr $i]
        set l [string first = $q]
        if {$l > 0} {
            set t [string range $q 0 [expr $l - 1]]
            set v [string range $q [expr $l + 1] end]
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
    if {[tk4]} {
        listbox $w.top.use.list -width 26 \
                -yscrollcommand "$w.top.use.scroll set"
    } else {
        listbox $w.top.use.list -geometry 26x10 \
                -yscrollcommand "$w.top.use.scroll set"
    }
    scrollbar $w.top.use.scroll -orient vertical -border 1
    pack $w.top.use.label -side top -fill x \
            -padx 2 -pady 2
    pack $w.top.use.list -side left -fill both -expand yes \
            -padx 2 -pady 2
    pack $w.top.use.scroll -side right -fill y \
            -padx 2 -pady 2
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
    bottom-buttons $w [list \
            {Ok} [list index-setup-action $attr $queryNo $indexNo] \
            {Cancel} [list destroy $w]] 0

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
    global queryInfoTmp
    global queryButtonsTmp
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
# Makes a dialog in which a query type an be customized.
proc query-setup {queryNo} {
    set w .query-setup

    global queryTypes
    global queryButtons
    global queryInfo
    global queryButtonsTmp
    global queryInfoTmp
    global queryIndexTmp
    
    set queryIndexTmp 0
    set queryName [lindex $queryTypes $queryNo]
    set queryInfoTmp [lindex $queryInfo $queryNo]
    set queryButtonsTmp [lindex $queryButtons $queryNo]

    toplevelG $w

    wm minsize $w 0 0
    wm title $w "Query setup $queryName"

    top-down-window $w

    frame $w.top.lines -relief ridge -border 2

    pack $w.top.lines -side left -pady 6 -padx 6 -fill y

    # Index Lines

    index-lines $w.top.lines 0 $queryButtonsTmp $queryInfoTmp activate-e-index

    button $w.top.lines.add -text "Add" \
            -command [list query-add-line $queryNo]
    button $w.top.lines.del -text "Remove" \
            -command [list query-del-line $queryNo]

    pack $w.top.lines.del -fill x -side bottom
    pack $w.top.lines.add -fill x -pady 10 -side bottom

    # Indexes

    frame $w.top.index -relief ridge -border 2
    pack $w.top.index -pady 6 -padx 6 -side right -fill y

    listbox $w.top.index.list -yscrollcommand [list $w.top.index.scroll set]
    scrollbar $w.top.index.scroll -orient vertical -border 1 \
        -command [list $w.top.index.list yview]
    bind $w.top.index.list <Double-1> [list query-edit-index $queryNo]

    pack $w.top.index.list -side left -fill both -expand yes -padx 2 -pady 2
    pack $w.top.index.scroll -side right -fill y -padx 2 -pady 2

    if {[tk4]} {
        $w.top.index.list selection clear 0 end
        $w.top.index.list selection set 0 0
    } else {
        $w.top.index.list select from 0
        $w.top.index.list select to 0
    }

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
    global queryButtonsFind
    global queryInfoFind

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
                set term [string range $term 0 [expr $len - 1]]
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
    dputs "qs=  $qs"
    return $qs
}

# Procedure index-focus-in {w i}
#  w    index frame
#  i    index number
# This procedure handles <FocusIn> events. A red border is drawed
# around the active search entry field when tk3.6 is used (tk4.X
# makes a black focus border itself).
proc index-focus-in {w i} {
    global curIndexEntry

    if {! [tk4]} {
        $w.$i configure -background red
    }
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
            if {[tk4]} {
                frame $w.$i -border 0
            } else {
                frame $w.$i -background white -border 1
            }
        }
        listbuttonx $w.$i.l [lindex $b 1] $queryInfo $handle $i

        if {$realOp} {
            if {! [winfo exists $w.$i.e]} {
                entry $w.$i.e -width 32 -relief sunken -border 1
                bind $w.$i.e <FocusIn> [list index-focus-in $w $i]
                bind $w.$i.e <FocusOut> [list $w.$i configure \
                        -background white]
                pack $w.$i.l -side left
                pack $w.$i.e -side left -fill x -expand yes
                pack $w.$i -side top -fill x -padx 2 -pady 2
                if {![tk4]} {
                    bind $w.$i.e <Left> [list left-cursor $w.$i.e]
                    bind $w.$i.e <Right> [list right-cursor $w.$i.e]
                }
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
    if {! [tk4]} {
        set j 0
        incr i -1
        while {$j < $i} {
            set k [expr $j+1]
            bind $w.$j.e <Tab> "focus $w.$k.e"
            set j $k
        }
    }
    if {$i >= 0} {
        if {! [tk4]} {
            bind $w.$i.e <Tab> "focus $w.0.e"
        }
        focus $w.0.e
    }
}

# Procedure search-fields {w buttondefs}
#  w           search fields entry frame
#  buttondefs  button definitions
# Makes search entry fields and listbuttons. 
# Note: This procedure is not used elsewhere. The index-lines
#       procedure is used instead.
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

# Init: The geometry information for the main window is set - either
# to a default value or to the value in windowGeometry(.)
if {[catch {set g $windowGeometry(.)}]} {
    wm geometry . 420x340
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

# Init: Definition of File menu.
menubutton .top.file -text File -menu .top.file.m
menu .top.file.m
.top.file.m add command -label {Save settings} -command {save-settings}
.top.file.m add separator
.top.file.m add command -label Exit -command {exit-action}

# Init: Definition of Target menu.
menubutton .top.target -text Target -menu .top.target.m
menu .top.target.m
.top.target.m add cascade -label Connect -menu .top.target.m.clist
.top.target.m add command -label Disconnect -command {close-target}
.top.target.m add command -label About -command {about-target}
.top.target.m add cascade -label Setup -menu .top.target.m.slist
.top.target.m add command -label {Setup new} -command {define-target-dialog}
.top.target.m add separator
set-target-hotlist 0

configure-disable-e .top.target.m 1
configure-disable-e .top.target.m 2

menu .top.target.m.clist
menu .top.target.m.slist
cascade-target-list

# Init: Definition of Service menu.
menubutton .top.service -text Service -menu .top.service.m
menu .top.service.m
.top.service.m add cascade -label Database -menu .top.service.m.dblist
.top.service.m add cascade -label Present -menu .top.service.m.present
menu .top.service.m.present
.top.service.m.present add command -label {10 More} \
        -command [list present-more 10]
.top.service.m.present add command -label All \
        -command [list present-more {}]
.top.service.m add command -label Search -command {search-request 0}
.top.service.m add command -label Scan -command {scan-request}

.top.service configure -state disabled

menu .top.service.m.dblist

menubutton .top.rset -text Set -menu .top.rset.m
menu .top.rset.m
.top.rset.m add command -label Load -command {load-set}
.top.rset.m add separator

# Init: Definition of the Options menu.
menubutton .top.options -text Options -menu .top.options.m
menu .top.options.m
.top.options.m add cascade -label Query -menu .top.options.m.query
.top.options.m add cascade -label Format -menu .top.options.m.formats
.top.options.m add cascade -label Wrap -menu .top.options.m.wrap
.top.options.m add cascade -label Syntax -menu .top.options.m.syntax
.top.options.m add cascade -label Elements -menu .top.options.m.elements
.top.options.m add radiobutton -label Debug -variable debugMode -value 1

# Init: Definition of the Options|Query menu.
menu .top.options.m.query
.top.options.m.query add cascade -label Select \
        -menu .top.options.m.query.clist
.top.options.m.query add cascade -label Edit \
        -menu .top.options.m.query.slist
.top.options.m.query add command -label New \
        -command {query-new}
.top.options.m.query add cascade -label Delete \
        -menu .top.options.m.query.dlist

menu .top.options.m.query.slist
menu .top.options.m.query.clist
menu .top.options.m.query.dlist
cascade-query-list

# Init: Definition of the Options|Formats menu.
menu .top.options.m.formats
set i 0
foreach f $displayFormats {
    .top.options.m.formats add radiobutton -label $f -value $i \
            -command [list set-display-format $i] -variable displayFormat
    incr i
}

# Init: Definition of the Options|Wrap menu.
menu .top.options.m.wrap
.top.options.m.wrap add radiobutton -label Character \
        -value char -variable textWrap -command {set-wrap char}
.top.options.m.wrap add radiobutton -label Word \
        -value word -variable textWrap -command {set-wrap word}
.top.options.m.wrap add radiobutton -label None \
        -value none -variable textWrap -command {set-wrap none}

# Init: Definition of the Options|Syntax menu.
menu .top.options.m.syntax
.top.options.m.syntax add radiobutton -label None \
        -value None -variable recordSyntax
.top.options.m.syntax add separator
.top.options.m.syntax add radiobutton -label USMARC \
        -value USMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label UNIMARC \
        -value UNIMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label UKMARC \
        -value UKMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label DANMARC \
        -value DANMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label FINMARC \
        -value FINMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label NORMARC \
        -value NORMARC -variable recordSyntax
.top.options.m.syntax add radiobutton -label PICAMARC \
        -value PICAMARC -variable recordSyntax
.top.options.m.syntax add separator
.top.options.m.syntax add radiobutton -label SUTRS \
        -value SUTRS -variable recordSyntax
.top.options.m.syntax add separator
.top.options.m.syntax add radiobutton -label GRS1 \
        -value GRS1 -variable recordSyntax

# Init: Definition of the Options|Elements menu.
menu .top.options.m.elements
.top.options.m.elements add radiobutton -label Unspecified \
        -value None -variable elementSetNames
.top.options.m.elements add radiobutton -label Full \
        -value F -variable elementSetNames
.top.options.m.elements add radiobutton -label Brief \
        -value B -variable elementSetNames

# Init: Definition of Help menu.
menubutton .top.help -text "Help" -menu .top.help.m
menu .top.help.m

.top.help.m add command -label "Help on help" \
        -command {tkerror "Help on help not available. Sorry"}
.top.help.m add command -label "About" -command {about-origin}

# Init: Pack menu bar items.
pack .top.file .top.target .top.service .top.rset .top.options -side left
pack .top.help -side right

# Init: Define query area.
index-lines .lines 1 $queryButtonsFind [lindex $queryInfo 0] activate-index

button .mid.search -text Search -command {search-request 0} \
        -state disabled
button .mid.scan -text Scan \
        -command scan-request -state disabled 
button .mid.present -text {Present} -command [list present-more 10] \
        -state disabled

button .mid.clear -text Clear -command index-clear
pack .mid.search .mid.scan .mid.present .mid.clear -side left \
        -fill y -pady 1

# Init: Define record area in main window.
text .data.record -font fixed -height 2 -width 20 -wrap none -borderwidth 0 \
        -relief flat -yscrollcommand [list .data.scroll set] -wrap $textWrap
scrollbar .data.scroll -command [list .data.record yview]
if {[tk4]} {
    .data.record configure -takefocus 0
    .data.scroll configure -takefocus 0
}

pack .data.scroll -side right -fill y
pack .data.record -expand yes -fill both
initBindings

# Init: Define standards tags. These are used in the display
# format procedures.
if {! $monoFlag} {
    .data.record tag configure marc-tag -foreground blue
    .data.record tag configure marc-id -foreground red
} else {
    .data.record tag configure marc-tag -foreground black
    .data.record tag configure marc-id -foreground black
}
.data.record tag configure marc-data -foreground black
.data.record tag configure marc-head \
        -font -Adobe-Times-Bold-R-Normal-*-140-* \
        -foreground brown -relief raised -borderwidth 1
.data.record tag configure marc-small-head -foreground brown
.data.record tag configure marc-pref \
        -font -Adobe-Times-Medium-R-Normal-*-140-* \
        -foreground blue
.data.record tag configure marc-text \
        -font -Adobe-Times-Medium-R-Normal-*-140-* \
        -foreground black
.data.record tag configure marc-it \
        -font -Adobe-Times-Medium-I-Normal-*-140-* \
        -foreground black

# Init: Define logo.
button .bot.logo -bitmap @${libdir}/bitmaps/book1 -command cancel-operation
if {[tk4]} {
    .bot.logo configure -takefocus 0
}
# Init: Define status information fields at the bottom.
frame .bot.a
pack .bot.a -side left -fill x
pack .bot.logo -side right -padx 2 -pady 2 -ipadx 1

message .bot.a.target -text "" -aspect 1000 -border 1

label .bot.a.status -text "Not connected" -width 15 -relief \
        sunken -anchor w -border 1
label .bot.a.set -text "" -width 5 -relief \
        sunken -anchor w -border 1
label .bot.a.message -text "" -width 15 -relief \
        sunken -anchor w -border 1

pack .bot.a.target -side top -anchor nw -padx 2 -pady 2
pack .bot.a.status .bot.a.set .bot.a.message \
        -side left -padx 2 -pady 2 -ipadx 1 -ipady 1

# Init: Determine if the IrTcl extension is already there. If
#  not, then dynamically load the IrTcl extension.
if {[catch {ir z39}]} {
    set e [info sharedlibextension]
    puts -nonewline "Loading irtcl$e ..."
    load irtcl$e irtcl
    ir z39
    puts "ok"
}

if {[file exists ${libdir}/explain.tcl]} {
    source ${libdir}/explain.tcl
}

if {[file exists ${libdir}/setup.tcl]} {
    source ${libdir}/setup.tcl
}

# Init: Uncomment this line if you wan't to enable logging.
ir-log-init all

# Init: If hostid is a valid target, a new connection will be established
# immediately.
if {[string compare $hostid Default]} {
    catch {open-target $hostid $hostbase}
}

# Init: Enable the logo.
show-logo 1


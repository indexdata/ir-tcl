# Setup file
set {profile(io)} {IO io.dtv.dk 9999 {} 50000 30000 tcpip Default 1 {} {} Z39 34 2 0 0 4 851083005 {} {} {} {} {} {} {} {} {} {} {}}

set {profile(Penn)} {{Penn State's Library} 128.118.88.200 210 {} 16384 8192 tcpip CATALOG 1 {} {} Z39 2 {} {} {} {} {} 847978115 {} {} {} {} {} {} {} {} {} {}}

set {profile(DanBib, SR)} {{SR Target DANBIB} 0103/find2.denet.dk 4500 {} 8192 8192 mosi danbib 1 {} 1 SR 8 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(AGRICOLA)} {AGRICOLA Tikal.dev.oclc.org 210 {} 50000 30000 tcpip AGRICOLA 1 {} {} Z39 31 2 0 0 4 {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(madison)} {{University of Wisconsin-Madison} z3950.adp.wisc.edu 210 {} 16384 8192 tcpip madison 1 {} {} Z39 22 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(bibsys)} {{BIBSYS Target (YAZ-based)} z3950.bibsys.no 2100 {} 16384 8192 tcpip BIBSYS 1 {} 1 Z39 27 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(Default)} {{} {} {210} {} 50000 30000 tcpip {} 1 {} {} Z39 35 2 0 0 4 {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(RLG)} {{Research Libraries group} rlg.stanford.edu 210 {} 32768 32768 tcpip {DEM} 1 {} 1 Z39 5 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(ztest9999)} {{YAZ server on localhost} localhost 9999 {} 50000 30000 tcpip Default {} {} {} Z39 33 2 0 0 4 842607655 858590340 842611107 {} {} {} {} {} {} {} {} {}}

set {profile(AT&T server)} {{AT&T Z39 Server} z3950.research.att.com 210 {} 90000 90000 tcpip {explain books gils netlib ftp z39dbs ahd books books books factbook russian outside-marc} 1 {} {} Z39 21 {} {} {} {} {} 842605350 842605239 {Lucent Technologies Research Server} {} 100 600000 {} {} 0 {Salutations - this is Lucent Technologies experimental Z39.50 server. No guarentees, but free and unlimited access!} {}}

set {profile(LOC)} {{Library of Congress} IBM2.LOC.gov 2210 {} 16384 16384 tcpip {BOOKS NAMES} 1 {} 0 Z39 6 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(DanBib)} {{Danish Union Catalogue} ir.dbc.bib.dk 2008 {} 50000 30000 tcpip danbib 1 {} {} Z39 32 2 0 0 4 {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(CARL)} {{CARL systems} z3950.marmot.org 210 {} 32768 32768 tcpip {ADA ASP CMC CNW DUR EAG LEW MST MPL MPS MON PTH PTK SWL VAI PVS COR SUM THR GAR SMG BUD CRM DEL GUN} 1 {} {} Z39 11 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(CLSI)} {CLSI inet-gw.clsi.us.geac.com 210 {} 16384 8192 tcpip cl_default 1 {} {} Z39 13 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set {profile(AULS)} {{Acadia university
} auls.acadiau.ca 210 {} 16384 8192 tcpip {AULS mad} 1 {} {} Z39 14 {} {} {} {} {} {} {} {
} {
} {} {} {} {} {} {} {}}

set {profile(dranet)} {dranet dranet.dra.com 210 {} 16384 16384 tcpip drewdb 1 {} 1 Z39 15 {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}}

set queryTypes {Simple phrase}
set queryButtons {{{I 3} {I 0} {I 0}} {{I 0} {I 1} {I 0}}}
set queryInfo {{ {Title {1=4}} {Author {1=1}} {Subject {1=21}} {Any {1=1016}} {Query 1=1016 2=102} {Title-rank 1=4 2=102} {Date/time 1=1012} {Title-regular 1=4 2=3 4=2 5=102}} {{Title 1=4 4=1 6=2} {Author 1=1003 4=1 6=2} {ISBN 1=7} {ISSN 1=8} {Year 1=30 4=4 6=2} {Any {}}}}

# Setup file
set {profile(zserver)} {Zserver localhost 8889 {} 90000 90000 tcpip {esdd Default} 1 {} 1 Z39 28}
set {profile(Penn)} {{Penn State's Library} 128.118.88.200 210 {} 16384 8192 tcpip CATALOG 1 {} {} Z39 2}
set {profile(ztest)} {{test server} localhost 210 {} 60000 60000 tcpip dummy 1 {} 1 Z39 3}
set {profile(madison)} {{University of Wisconsin-Madison} z3950.adp.wisc.edu 210 {} 16384 8192 tcpip madison 1 {} {} Z39 22}
set {profile(bibsys)} {{BIBSYS Target (YAZ-based)} z3950.bibsys.no 2100 {} 16384 8192 tcpip BIBSYS 1 {} 1 Z39 27}
set {profile(Default)} {{} {} {210} {} 50000 30000 tcpip {} 1 {} {} {} 29 2 0 0 4}
set {profile(RLG)} {{Research Libraries group} rlg.stanford.edu 210 {} 4096 4096 tcpip {BKS AMC MAPS MDF REC SCO SER VIM NAF SAF AUT CATALOG ABI AVI DSA EIP FLP HAP HST NPA PAI PRA WLI} 1 {} 1 Z39 5}
set {profile(AT&T server)} {{AT&T Z39 Server} z3950.research.att.com 210 {} 16384 16384 tcpip Default 1 {} {} Z39 21}
set {profile(LOC)} {{Library of Congress} IBM2.LOC.gov 2210 {} 16384 16384 tcpip {BOOKS NAMES} 1 {} 0 Z39 6}
set {profile(DANBIB)} {{SR Target DANBIB} 0103/find2.denet.dk 4500 {} 8192 8192 mosi danbib 1 {} 1 SR 8}
set {profile(OCLC)} {{OCLC First search engine} z3950.oclc.org 210 {} 16384 8192 tcpip {ArticleFirst BiographyIndex BusinessPeriodicalsIndex} 1 {} {} Z39 9}
set {profile(CARL)} {{CARL systems} Z3950.carl.org 210 {} 16384 8192 tcpip {ACC AIC AUR BEM CUB DPL DNU EPL FRC LAW LCC MCC MIN MPL NJC NWC OCC PPC PUE RDR RGU SPL TCC TKU UNC WYO} 1 {} {} Z39 11}
set {profile(adad)} {a {} 210 {} 16384 8192 tcpip {} 1 {} {} Z39 26}
set {profile(Innovative)} {{Innovatives server: demo.iii.com} demo.iii.com 210 {} 16384 8192 tcpip DEFAULT 1 {} {} Z39 12}
set {profile(CLSI)} {CLSI inet-gw.clsi.us.geac.com 210 {} 16384 8192 tcpip cl_default 1 {} {} Z39 13}
set {profile(AULS)} {{Acadia university} auls.acadiau.ca 210 {} 16384 8192 tcpip AULS 1 {} {} Z39 14}
set {profile(canberra)} {canberra canberra.cs.umass.edu 2110 {} 30000 30000 tcpip cacm_dots 1 {} {} Z39 25}
set {profile(dranet)} {dranet dranet.dra.com 210 {} 16384 16384 tcpip drewdb 1 {} 1 Z39 15}
set queryTypes {Simple phrase}
set queryButtons {{{I 3} {I 0} {I 1} {I 2}} {{I 0} {I 1} {I 0}}}
set queryInfo {{ {Title {1=4}} {Author {1=1}} {Subject {1=21}} {Any {1=1016}} {Query 1=1016 2=102} {Title-rank 1=4 2=102} {Date/time 1=1012} {Title-regular 1=4 2=3 4=2 5=102}} {{Title 1=4 4=1 6=2} {Author 1=1003 4=1 6=2} {ISBN 1=7} {ISSN 1=8} {Year 1=30 4=4 6=2} {Any {}}}}

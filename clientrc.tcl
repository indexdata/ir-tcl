# Setup file
set {profile(CNIDR)} {CNIDR Kudzu.cnidr.org 5556 {} 16384 8192 tcpip {Book ERIC} 1 {} {} z39v2 1}
set {profile(Penn)} {{Penn State's Library} 128.118.88.200 210 {} 16384 8192 tcpip CATALOG 1 {} {} z39v2 2}
set {profile(A new server)} {{A completely new server} dtbsun.dtv.dk 9999 {} 16384 8192 tcpip dummy {} {} {} z39v2 20}
set {profile(ztest)} {{test server} localhost 9999 {} 16384 4096 tcpip dummy 1 {} {} z39v2 3}
set {profile(Nsrtest)} {{NSR in house.} localhost 4500 {} 16384 8192 mosi x 1 {} {} sr 4}
set {profile(Default)} {{} {} {210} {} 16384 8192 tcpip {} {} {} {} {} 22}
set {profile(RLG)} {{Research Libraries group} rlg.stanford.edu 210 {} 16384 16384 tcpip {BKS AMC MAPS MDF REC SCO SER VIM NAF SAF AUT CATALOG ABI AVI DSA EIP FLP HAP HST NPA PAI PRA WLI} 1 {} {} z39v2 5}
set {profile(AT&T server)} {{AT&T Z39 Server} z3950.research.att.com 210 {} 16384 8192 tcpip Default {} {} {} z39v2 21}
set {profile(LOC)} {{Library of Congress} IBM2.LOC.gov 210 {} 16384 16384 tcpip {BOOKS NAMES} 1 {} 0 z39v2 6}
set {profile(IREG)} {{Internet Resource} frost.notis.com 210 {} 16384 8192 tcpip {IREG ERIC} 1 {} {} z39v2 7}
set {profile(DANBIB)} {{SR Target DANBIB} 0103/find2.denet.dk 4500 {} 8192 8192 mosi danbib 1 {} 1 z39v2 8}
set {profile(OCLC)} {OCLC rdsd-rs6000.dev.oclc.org 211 {} 16384 8192 tcpip OLUC 1 {} {} z39v2 9}
set {profile(Aleph)} {{Aleph at ram10.aleph.co.il:5555} localhost 9998 {} 16384 4096 tcpip {dem mar} 1 0 1 z39v2 10}
set {profile(CARL)} {{CARL systems} Z39.50.carl.org 210 {} 16384 8192 tcpip {ACC AIC AUR BEM CUB DPL DNU EPL FRC LAW LCC MCC MIN MPL NJC NWC OCC PPC PUE RDR RGU SPL TCC TKU UNC WYO} 1 {} {} z39v2 11}
set {profile(Innovative)} {{Innovatives server: demo.iii.com} demo.iii.com 210 {} 16384 8192 tcpip DEFAULT 1 {} {} z39v2 12}
set {profile(CLSI)} {CLSI inet-gw.clsi.uc.geac.com 210 {} 16384 8192 tcpip Cl 1 {} {} z39v2 13}
set {profile(AULS)} {{Acadia university} auls.acadiau.ca 210 {} 16384 8192 tcpip AULS 1 {} {} z39v2 14}
set {profile(dranet)} {dranet dranet.dra.com 210 {} 16384 16384 tcpip drewdb 1 {} {} z39v2 15}
set queryTypes {Simple aaaaaaa}
set queryButtons {{ {I 0} {I 1} {I 2} } {{I 0} {I 1}}}
set queryInfo {{ {Title {1=4}} {Author {1=1}} {Subject {1=21}} {Any {1=1016}}} {{Title 1=4} {Year 1=30}}}

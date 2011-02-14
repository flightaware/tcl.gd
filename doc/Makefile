


all:	tclgd.ps tclgd_newGDObjectAttach.ps

tclgd.ps:	tclgd.n
	groff -mandoc -Tps tclgd.n >tclgd.ps

tclgd_newGDObjectAttach.ps: tclgd_newGDObjectAttach.3
	groff -mandoc -Tps tclgd_newGDObjectAttach.3 >tclgd_newGDObjectAttach.ps


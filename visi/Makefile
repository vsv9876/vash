#
# assembly distribution tarball (obsoleted)
#

TARF    = tar cvbsf 1 -


DTAPE   = \
	_base.tar    \
	_doc.tar     \
	_usr.tar     \
	_src.tar     \
	_rt11.tar    \
	_unused.tar  \
	_pp.tar

DIS     = \
	Makefile                \
	READ_ME                 \
	__LIST                  \
	_base.tar               \
	_doc.tar                \
	_usr.tar

all: $(TAR)
	echo $(TAR);

_base.tar:   _base
	grep _base __LIST | $(TARF) _base.tar;

_doc.tar:    _doc
	grep _doc __LIST | $(TARF) _doc.tar;

_usr.tar:    _usr
	grep _usr __LIST | $(TARF) _usr.tar;

_prog.tar:   _prog
	grep _prog __LIST | $(TARF) _prog.tar;

_src.tar:    _src
	grep _src __LIST | $(TARF) _src.tar;

_pp.tar: _pp
	grep _pp __LIST | $(TARF) _pp.tar;

_unused.tar: _unused
	grep _unused __LIST | $(TARF) _unused.tar;

_rt11.tar:   _rt11
	grep _rt11 __LIST | $(TARF) _rt11.tar;

_main.tar:   _base _doc _usr
	egrep '_base|_doc|_usr' __LIST | $(TARF) _main.tar

dis:    $(DIS)
	tar cpvB 240 $(DIS)

litle:  _base.tar _doc.tar _usr.tar

flop8:  _main.tar _src.tar _unused.tar _rt11.tar _pp.tar
	tar cvB 240 _main.tar _src.tar _unused.tar _rt11.tar _pp.tar

flop5:  __LIST _base _doc _usr
	egrep '_base|_doc|_usr' | tar cvBs - 400

tape:   __LIST _base _bin _doc _src _unused _rt11 _pp
	cat __LIST | tar cvs -

.DEFAULT:
	mkdir $@

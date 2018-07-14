/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *  New version for SYSTEM V termio.h.
 *     modify for 386bsd
 */

#include <stdio.h>
#include "line.h"

#ifndef USE_TERMIOS
/*
**      УПРАВЛЕНИЕ РЕЖИМАМИ ДРАЙВЕРА ТЕРМИНАЛА
**/

#ifdef USE_SGTTYH
#include <sgtty.h>
#else

#ifdef USE_TERMIO
#include       <termio.h>    /* UNIX 3.2 */
#endif  /*TERMIOS */

#ifdef  DEMOS2
#include <signal.h>

#ifdef  USE_TERMIOS_386BSD              /* 386bsd */
#include       <sys/types.h>
#include       <termios.h>
#include       <sys/ioctl.h>
#define termio termios
#define        TCGETA TIOCGETA
#define        TCSETA TIOCSETA
#define USE_TERMIO
#define CDEL   0377
#endif         /* TERMIOS */

#endif  /* USE_TERMIO */
/*
 *  #ifndef USE_TERMIO
 *  #error
 *  #endif
 */

#ifdef  RT11
static  int     *jsw = 044;     /* JOB STATUS WORD */
#endif

#ifdef USE_TERMIO
static struct termio old, new;  /* terminal i/o status */
#else
static struct sgttya old, new;          /* состояния терминала */
#endif /* USE_TERMIO */
#endif /* DEMOS2 */

int osgflg = 0;         /* ФЛАГИ ДРАЙВЕРА (old.sg_flags) */

/*------------------------------*/
/* РЕАКЦИЯ НА СИГНАЛ ПРЕРЫВАНИЯ */
/*------------------------------*/
#ifdef RT11
/* К СОЖАЛЕНИЮ, ДЛЯ RT-11 ПОКА НЕ РАБОТАЕТ ЗАПРОС .SCCA
 * ТАК, КАК ЭТОГО ХОТЕЛОСЬ БЫ...
 */
#endif

#ifdef DEMOS2
static v_off();
static void onintr()
{
	signal(SIGINT,SIG_IGN); /* ОСТАЛЬНЫЕ НОТЫ ШОПЕНА ИГНОРЕ... */
	v_off(); exit(0);
}
#endif

/*------------------------------*/
/* ТЕЛЕТАЙПНЫЙ РЕЖИМ - СТАНДАРТ */
/*------------------------------*/
static v_off()
{
#ifdef  DEMOS2
	static   void     (*fsig)();
#endif
#ifdef  RT11
	FILE *freopen() ;
#endif

	fflush(vttout);         /* СНАЧАЛА ЗАКОНЧИТЬ ВЫВОД */

#ifdef  RT11
	*jsw &= ~010100 ;                /* SPECIAL .TTINR MODE OFF */
	freopen("tt:", "w", vttout);     /* UNBUFERED I/O OFF */
#ifdef  decus
	vttout->_flag &= ~_IONBF ;       /* SINGLE CHAR OUTPUT OFF */
#endif  /* decus */
#endif  /* RT11 */

#ifdef  DEMOS2
	/* СНЯТЬ РЕАКЦИЮ НА CTRL-C */
	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == &onintr ) {
		fsig = SIG_DFL;
	}
	signal( SIGINT, fsig );

#ifdef USE_TERMIO
	ioctl(vtti, TCSETA, &old);
#else
	ioctl(vtti, TIOCSETB, &old);
#endif  /* USE_TERMIO */
#endif /* DEMOS */
}

/*----------------------------*/
/* УСТАНОВКА ЭКРАННОГО РЕЖИМА */
/*----------------------------*/
static v_on()
{
	register int i;

#ifdef  RT11
	FILE *freopen() ;

	*jsw |= 010100 ;                /* SPECIAL .TTINR MODE          */
	freopen ( "tt:", "wu", vttout ) ;       /* UNBUFERED I/O        */
#ifdef  decus
	vttout->_flag |= _IONBF ;       /* SINGLE CHAR OUTPUT           */
#endif  /* decus */
#endif  /* RT11 */

#ifdef  DEMOS2
	static   void       (*fsig)();

#ifndef USE_TERMIO
	ioctl(vtti,TIOCGETA,&old);         /* БЕРЕМ СТАРЫЕ РЕЖИМЫ */
	new = old;
	osgflg = old.sg_flags;          /* ЕКСПОРТИРУЕМ ФЛАГИ ДРАЙВЕРА */
/*      new.sg_flags = (old.sg_flags &
 *          ~(CRMOD|ECHO|XTABS|
 *          CYRILL|LCASE|UCASE)) | CBREAK ;
 */
	new.sg_flags = (old.sg_flags &
	    ~(ECHO|XTABS
/*                      |LCASE|UCASE          */
			)) | CBREAK ;
	if(old.sg_flags & LCASE)
		new.sg_flags &= ~UCASE;
	new.sg_width  = 0;
	new.sg_length = 0;
	/*      new.sg_quitc  = 0;      */
	/*      new.sg_startc = 0;      */
	/*      new.sg_stopc  = 0;      */
	new.sg_suspc  = 0;
	new.sg_dsuspc = 0;
	new.sg_rprntc = 0;
	/*      new.sg_flushc = 0;      */
	new.sg_werasc = 0;
	new.sg_lnextc = 0;
	new.sg_brkc   = 0;
	new.sg_eofc   = 0;

#else   /* #ifndef USE_TERMIO */
#ifdef  USE_TERMIO
	ioctl(vtti,TCGETA,&old);        /* get prev tty modes */
	new = old;
/*        osgflg = old.c_cflag;          /* ЕКСПОРТИРУЕМ ФЛАГИ ДРАЙВЕРА */

/*        new.c_iflag = INPCK&new.c_iflag;      */
	new.c_oflag = (new.c_oflag&(~OPOST));
/*        new.c_cflag = new.c_cflag&CREAD; */
	new.c_lflag = ISIG;

	/* special character processing */
#ifdef NCCS
	for ( i = 0; i <= NCCS; i++ )
		new.c_cc[i] = CDEL;
#else
#ifdef  NCC
	for ( i = 0; i <= NCC; i++ )
		new.c_cc[i] = CDEL;
#else

/*        new.c_cc[VINTR] = CNUL; */
/*        new.c_cc[VQUIT] = CNUL; */
	new.c_cc[VERASE]= CDEL;
	new.c_cc[VKILL] = CDEL;
	new.c_cc[VEOF]  = CDEL;
	new.c_cc[VEOL]  = CDEL;
	new.c_cc[VEOL2] = CDEL;
#ifdef USE_TERMIOS_386BSD
	new.c_cc[VDISCARD] = CDEL;
	new.c_cc[VLNEXT] = CDEL;
	new.c_cc[VWERASE]= CDEL;
	new.c_cc[VREPRINT]= CDEL;
	new.c_cc[VSUSP] = CDEL;
	new.c_cc[VDSUSP]= CDEL;
#else
	new.c_cc[VSWTCH]= CDEL;
#endif
	if(old.c_cc[VINTR] == 0177)
		new.c_cc[VINTR] = CDEL;
	new.c_cc[VMIN ] = 1;
	new.c_cc[VTIME] = 2;

#endif /* NCC */
#endif /* NCCS */
	if(old.c_cc[VINTR] == 0177)     /* непонятно, какой-то старый глюк */
		new.c_cc[VINTR] = CDEL;

	/* XON/XOFF characters */
#ifdef  VSTART
	new.c_cc[VSTART] = old.c_cc[VSTART];
	new.c_cc[VSTOP]  = old.c_cc[VSTOP];
#endif

	new.c_cc[VMIN ] = 1;
	new.c_cc[VTIME] = 2;

#endif /* USE_TERMIO */
#endif /* !USE_TERMIO */

	/* РЕАКЦИЯ НА CTRL-C */
	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == SIG_DFL )   {
		fsig = &onintr;
		signal( SIGINT, fsig );
	}
#ifndef USE_TERMIO
	ioctl(vtti, TIOCSETB, &new);
#else
	ioctl(vtti, TCSETA, &new);
#endif  /* TERMIO */
#endif  /* DEMOS2 */
}

#ifdef  DEMOS2
/*-----------------------------------------------------*/
/* ПРОВЕРКА НА ОТСУТСТВИЕ СИМВОЛОВ В БУФЕРЕ КЛАВИАТУРЫ */
/*-----------------------------------------------------*/
ttytst()
{
	static long num_r;
#ifndef USE_TERMIO
	/* 386bsd, USE_TERMIOS */
	ioctl(vtti, FIONREAD, &num_r);
#else
#ifdef USE_TERMIOS_386BSD
	/* 386bsd, USE_TERMIOS_386BSD */
	ioctl(vtti, FIONREAD, &num_r);
#else
	/* USE_TERMIO */
	ioctl(vtti, FIORDCHK, &num_r);
#endif /* USE_TERMIOS_386BSD */
#endif  /* USE_TERMIO */
	return( num_r == 0l );
}
#endif

/*--------------*/
/* ВВОД СИМВОЛА */
/*--------------*/
/* Для драйвера ДЕМОС требуется отслеживать ввод,
 * и если принят символ русского регистра, то
 * на тех терминалах, где клавиятура управляется от ЭВМ,
 * требуется скорректировать ее состояние
 * до ввода следующего символа.
 *      P.S.
 *      Поскольку такие терминалы сами по себе продукт
 *      еще тот, то и их поддержка того же уровня -
 *      Здесь результат освоения документации без отладки.
 */

int     cyrflg = 0;     /* последний символ пришел латинский */

static  int cyrkbd()
{
#ifdef DEMOS2
#ifdef DEMOS2_CYR
	if( (osgflg & LCASE) )
		return;
	if(cyrflg && ((osgflg & CYRILL) != 0))
		switch(osgflg & CSTYLE) {
		case CS_TYPE2 :
		case CS_TYPE0 :
			putc('N' & 037, vttout);  /* выдать SI (ctrl/n) */
			break;
		};
#endif /* USE_TERMIO */
#endif
}

#ifdef  RT11
/* lin110.mac: ИМИТАЦИЯ ЗАПРОСА .TTINR RT-11 */
extern  int     ttinr();
#endif

static int kb_nwt = 0;  /* Не ожидать ввод с клавиатуры -
			 * при пустой очереди выполнять
			 * асинхронные задания.
			 * ПО УМОЛЧАНИЮ - ВВОД ОЖИДАЕТСЯ */

/* Вернуть принятый символ, без символа возврата нет */
/* асинхронные задания выполняются в фоне, если нужно */
ttyinp()
{
	char         cc;
	register int c;

	next_j();       /* ОДИН РАЗ ПЕРЕД ВВОДОМ КАЖДОГО СИМВОЛА */
	cyrkbd();
	fflush(vttout);

	if(kb_nwt) {
#ifdef  RT11
		while((c=ttinr()) == -1) {  /* пока символ не принят */
			next_j();       /* прокрутить асинхронщину */
			cyrkbd();
			fflush(vttout);
		}
#endif
#ifdef  DEMOS2
		while( ttytst() ) { /* ПОКА оЧЕРЕДЬ ПУСТАЯ */
			next_j();       /* ПРОКРУТИТЬ АСИНХРОНЩИНУ */
			cyrkbd();
			fflush(vttout);
		}
		read(vtti, &cc, 1);
		c = cc;
#endif
		goto ret_c;     /* вернуть код */
	} else {
#ifdef  RT11
		while( (c=ttinr()) == -1);      /* while for SJ only */
#endif

#ifdef  DEMOS2
		read(vtti, &cc, 1);        /* символ прочитать */
		c = cc;
		goto ret_c;
#endif
ret_c:
		c &= 0377;
		if(c > 0177)    cyrflg = 1;
		else            cyrflg = 0;
		return( c );
	}
}

/*------------------------------------------------*/
/* УПРАВЛЕНИЕ РЕЖИМАМИ ТЕРМИНАЛЬНОГО ВВОДА-ВЫВОДА */
/*------------------------------------------------*/

extern int k_pad();     /* см. lin215.c */

io_set(flags)
register int flags;
{
	/* переключение keypad приходится делать из-за
	 * повсеместного распространения termcap для vt100, xterm,
	 * в котором клавиши со стрелками правильно работают
	 * только при включенном keypad
	 */
	if(flags & IO_VIDEO)    {
		v_on();
		k_pad(1);
	}
	if(flags & IO_TTYPE)    {
		k_pad(0);
		v_off();
	}
	if(flags & IO_WAIT)
				kb_nwt = 0;
	if(flags & IO_NOWAIT)
				kb_nwt = 1;
}
#endif /* USE_TERMIOS */

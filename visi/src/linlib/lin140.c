/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *  New version for POSIX termios.h.
 */

#include <stdio.h>
#include "line.h"

#ifdef USE_TERMIOS
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>
/* #ifndef UW_20 */
/* #include <sys/filio.h> */
/* #endif */

int tty_li = 0;
int tty_co = 0;

/*
extern int maxli;
extern int maxco;
*/

struct winsize winsz;
int gtty_sz()
{
	int ret;
	ret = ioctl(vtti, TIOCGWINSZ, &winsz);
	if (ret == 0) {
		hwframe.maxli = tty_li = winsz.ws_row;
		hwframe.maxco = tty_co = winsz.ws_col;
	}
	return ret;
}


/*
 *  Управление режимами драйвера терминала
 */

static struct termios old;
static struct termios new;  /* terminal i/o status */

int osgflg = 0;         /* ФЛАГИ ДРАЙВЕРА (old.sg_flags)???????? */

/*
 * реакция на сигнал прерывания
 */
static v_off();
static void onintr()
{
	/* эта функция применяется как реакция на SIG_QUIT и при exit */
	signal(SIGINT,SIG_IGN);
	v_off(); exit(0);
}

/*------------------------------*/
/* ТЕЛЕТАЙПНЫЙ РЕЖИМ - СТАНДАРТ */
/*------------------------------*/
static int v_off()
{
	static   void     (*fsig)();

	fflush(vttout);         /* СНАЧАЛА ЗАКОНЧИТЬ ВЫВОД */

	/* вернуть стандартную реакцию на сигнал SIG_INT */
	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == &onintr ) {
		fsig = SIG_DFL;
	}
	signal( SIGINT, fsig );

#if 0
	ioctl(vtti, TCSETS, &old);
#endif
	/* tcsetattr(vtti, TCSADRAIN, &old); */
	return (tcsetattr(vtti, TCSANOW, &old));
}

/*----------------------------*/
/* УСТАНОВКА ЭКРАННОГО РЕЖИМА */
/*----------------------------*/
static int v_on(inherit)
int inherit;
{
	static   void       (*fsig)();
	cc_t i;

	if (inherit) {
		/* get prev tty modes */
#if 0
	ioctl(vtti,TCGETS,&old);        /* get prev tty modes */
#endif
		tcgetattr(vtti, &old);
		new = old;
		new.c_iflag = (new.c_iflag&(~(ICRNL|IGNCR)));
		new.c_oflag = (new.c_oflag&(~OPOST));
		new.c_lflag = ISIG;
#ifndef CNUL
#define CNUL '\0'
#endif
	/* special character processing */
/*        new.c_cc[VINTR  ] = CNUL; */
/*        new.c_cc[VQUIT  ] = CNUL; */
		for ( i = 0; i <= NCCS; i++)
			new.c_cc[i] = CNUL;
		new.c_cc[VSTART] = old.c_cc[VSTART];
		new.c_cc[VSTOP] = old.c_cc[VSTOP];
		new.c_cc[VMIN ] = 1;    /* TODO check, may be VMIN=VTIME=0 is better ? */
		new.c_cc[VTIME] = 5;
	}

	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == SIG_DFL )   {
		fsig = &onintr;
		signal( SIGINT, fsig );
	}

#if 0
	ioctl(vtti, TCSETS, &new);
#endif
	/* tcsetattr(vtti, TCSADRAIN, &new); */
	fflush(vttout);
	return(tcsetattr(vtti, TCSANOW, &new));
}

/*-----------------------------------------------------*/
/* ПРОВЕРКА НА ОТСУТСТВИЕ СИМВОЛОВ В БУФЕРЕ КЛАВИАТУРЫ */
/*-----------------------------------------------------*/
ttytst()
{
	static long num_r;
#ifdef FIONREAD
	ioctl(vtti, FIONREAD, &num_r);
#else
#ifndef FIORDCHK
#define FIORDCHK        (('f'<<8)|3)            /* V7 */
#endif
	ioctl(vtti, FIORDCHK, &num_r);
#endif
	return( num_r == 0l );
}

/*--------------*/
/* ВВОД СИМВОЛА */
/*--------------*/
/* Для драйвера ДЕМОС требуется отслеживать ввод,
 * и если принят символ русского регистра, то
 * на тех терминалах, где режим клавиатуры управляется
 * выводом символа на экран терминала (рус/лат или ^N/^O),
 * требуется скорректировать состояние ктавиатуры
 * до ввода следующего символа.
 *      P.S.
 *      Поскольку такие терминалы сами по себе продукт
 *      еще тот, то и их поддержка того же уровня -
 *      Здесь результат освоения документации без отладки.
 */

int     cyrflg = 0;     /* последний символ пришел латинский */

static  int cyrkbd()
{
/* оставлена заглушка для совместимости */
}

static int kb_nwt = 0;  /* Не ожидать ввод с клавиатуры -
			 * при пустой очереди выполнять
			 * асинхронные задания.
			 * ПО УМОЛЧАНИЮ - ВВОД ОЖИДАЕТСЯ */
/*
 * Вернуть принятый символ, без символа
 * асинхронные задания выполняются в фоне, если нужно
 */
#ifdef DEMOS_CYR
ttyinp()
{
	char         cc;
	register int c;

	next_j();       /* один раз перед вводом каждого символа */
	cyrkbd();
	fflush(vttout);

	if(kb_nwt) {
		while( ttytst() ) { /* пока очередь пустая */
			next_j();       /* прокрутить асинхронщину */
			cyrkbd();
			fflush(vttout);
		}
		if (read(vtti, &cc, 1) < 0) exit(2);
		c = cc;
		goto ret_c;     /* вернуть код */
	} else {

		if (read(vtti, &cc, 1) < 0) exit(1);
		c = cc;
		goto ret_c;
ret_c:
		c &= 0377;
		if(c > 0177)    cyrflg = 1;
		else            cyrflg = 0;
		return( c );
	}
}
#else

/* вообще-то эта функция читает поток мультибайтовых символов,
 * но требует асинхронного режима и читает не поток, а дескриптор файла;
 * чтение выполняется в режиме за одно обращение один байт -
 * сборка в codepoint (символ UTF-8) выполняется на уровень выше, в вызывающей функции */
ttyinp()
{
	char         cc;
	register int c;

	next_j();       /* один раз перед вводом каждого символа */
	cyrkbd();
	fflush(vttout);

	if(kb_nwt) {
		while( ttytst() ) { /* пока очередь ввода пустая */
			next_j();       /* выполнить другие асинхронные задания */
			/*
			 * в 1986 году мы с коллегами не знали что эта техника называется threads,
			 * а PosixThreads появились позже на несколько лет -- vsv, 2019г.
			 */
			cyrkbd();
			fflush(vttout);
		}
		if (read(vtti, &cc, 1) < 0) exit(2);
		c = cc;
		goto ret_c;     /* вернуть код */
	} else {

		if (read(vtti, &cc, 1) < 0) exit(1);
		c = cc;
		goto ret_c;
ret_c:
		c &= 0377;
		if(c > 0177)    cyrflg = 1;
		else            cyrflg = 0;
		return( c );
	}
}
#endif

/*------------------------------------------------*/
/* УПРАВЛЕНИЕ РЕЖИМАМИ ТЕРМИНАЛЬНОГО ВВОДА-ВЫВОДА */
/*------------------------------------------------*/

extern int k_pad();     /* см. lin215.c */

io_set(flags)
register int flags;
{
	int inherit = 1; /* by default save old tty state, or drop in case waiting killed child */
	/* переключение keypad приходится делать из-за
	 * повсеместного распространения termcap для vt100, xterm,
	 * в котором клавиши со стрелками правильно работают
	 * только при включенном keypad
	 */
	if(flags & IO_VIDEO)    {
		fflush(vttout);
		if (flags & IO_TTYSANE)
				inherit = 0;
		if (v_on(inherit)) {
			perror("io_set(v_on)");
			v_on(inherit);
		}
		k_pad(1);
	}
	if(flags & IO_TTYPE)    {
		k_pad(0);
		fflush(vttout);
		if (v_off()) {
			perror("io_set(v_off)");
			v_off();
		}
	}
	if(flags & IO_WAIT)
				kb_nwt = 0;
	if(flags & IO_NOWAIT)
				kb_nwt = 1;
}

#endif  /* USE_TERMIOS */

/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *  New version for POSIX termios.h.
 */

#include <unistd.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"

#ifdef USE_TERMIOS
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>

/* #ifndef UW_20 */
/* #include <sys/filio.h> */
/* #endif */

#ifdef VISI_DEBUG
static int debug = 0; /* env VISI_IO= */
static int debug_vt = 1;
#endif /*VISI_DEBUG*/

/* do not wait of input;
 * if no input cycle asinc jobs - call nxt_job();
 */
static int kb_nwt = 0; /* wait input by default */
static int vt_off(void);
static int vt_on(void);
static void vt_ini(void);

int gtty_sz()
{
	int ret;
	struct winsize wsz;

	ret = ioctl(vtti, TIOCGWINSZ, &wsz);
	if (ret == 0) {
		hwframe.maxli = /*tty_li =*/ wsz.ws_row;
		hwframe.maxco = /*tty_co =*/ wsz.ws_col;
	}
	return ret;
}

/*
 *  internal state of terminal i/o
 */

static struct termios t_vton;  /* terminal i/o status */
static struct termios t_vtoff;
static int s_vton = 0;	/* saved */
static int s_vtoff = 0;

/*static int l_vton = -1;
static int l_vtoff = -1;*/

/* int osgflg = 0;         /* NotUsed! ФЛАГИ ДРАЙВЕРА (old.sg_flags)???????? */

static void onintr(int);

static   void     (*fsig)();
/*TODO
static struct sigaction act_def = { onintr, NULL, 0, 0, NULL };
static struct sigation osig;
*/
/*
 * signal disposition for VIDEO mode
 */
void vsig_on(void) {
#if VISI_SIG_POSIX
	struct sigaction sa;
	sa.sa_handler = onintr;
	sa.sa_flags = SA_RESTART;

	sigemptyset(&sa.sa_mask);

	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
/*	sigaction(SIGTERM, &sa, NULL);*/
/*	sa.sa_handler = SIG_IGN;*/
/*	sigaction(SIGQUIT, &sa, NULL);*/

#else
	signal(SIGHUP, &onintr);
	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == SIG_DFL )   {
		fsig = &onintr;
		signal( SIGINT, fsig );
	}
#endif
}

/* for TTY mode */
void vsig_off(void) {
#if VISI_SIG_POSIX
	struct sigaction sa;
	sa.sa_handler = SIG_DFL/*onintr*/;
	sa.sa_flags = 0;

	sigemptyset(&sa.sa_mask);

	sigaction(SIGHUP,  &sa, NULL);
	sigaction(SIGINT,  &sa, NULL);
/*	sigaction(SIGTERM, &sa, NULL);*/
/*	sigaction(SIGQUIT, &sa, NULL);*/
#else
	/* backing LINLIB default reaction on signals */
	signal(SIGHUP, &onintr);
	fsig = signal(SIGINT,SIG_IGN);
	if( fsig == &onintr ) {
		fsig = SIG_DFL;
	}
	signal( SIGINT, fsig );
#endif
}

/*
 *  default library handler for SIG_INT, SIG_QUIT and used on exit
 *  */
static void onintr(sig)
int sig;
{
#if VISI_SIG_POSIX
	/*signal(SIGINT,SIG_IGN);*/
#else
	signal(SIGHUP, &onintr);
	signal(SIGINT, &onintr);
#endif
	vt_off();
/*	printf("-------- exited via onintr(%-d) --------\n", sig);*/
	fflush(stdout);
	exit(0);
}

/*------------------------------*/
/* ТЕЛЕТАЙПНЫЙ РЕЖИМ - СТАНДАРТ */
/*------------------------------*/
static int vt_off()
{
#ifdef VISI_DEBUG
	if (debug)
		printf(" ~vt_off~ ");
/*	fflush(vttout);         /* СНАЧАЛА ЗАКОНЧИТЬ ВЫВОД */
#endif /*VISI_DEBUG*/
	vsignal(1);

	/*ioctl(vtti, TCSETS, &old);*/
	/* tcsetattr(vtti, TCSADRAIN, &old); */
	fflush(stdout);
	return (tcsetattr(STDOUT_FILENO /*vtti*/, TCSADRAIN/*TCSANOW*/, &t_vtoff));
}

/*----------------------------*/
/* УСТАНОВКА ЭКРАННОГО РЕЖИМА */
/*----------------------------*/
static int vt_on()
{
#ifdef VISI_DEBUG
	if (debug)
		printf(" ~vt_on~ ");
	fflush(stdout);
/*	fflush(vttout);         /* СНАЧАЛА ЗАКОНЧИТЬ ВЫВОД */
#endif /*VTTOUT_DEBUG*/

	vsignal(1);

	/*ioctl(vtti, TCSETS, &old);*/
	/* tcsetattr(vtti, TCSADRAIN, &old); */
	fflush(stdout);
	return (tcsetattr(STDOUT_FILENO /*vtti*/, TCSADRAIN/*TCSANOW*/, &t_vton));
}

static void vt_ini()
{
	static   void       (*fsig)();
	cc_t i;

#ifdef VISI_DEBUG
	if (debug)
		printf(" ~vt_ini ");
	fflush(stdout);
#endif /*VISI_DEBUG*/

	/* get prev tty modes if ...
	 * it is intended for old code with first call io_set(IO_VIDEO);
	 * modern code have to use io_set(IO_SAVE) first;
	 */
	if (s_vtoff == 0) {
		tcgetattr(vtti, &t_vtoff);
		/* get copy of common flags from prepare settings for vt_on */
		memcpy(&t_vton, &t_vtoff, sizeof(t_vton));
		s_vton = s_vtoff = 1;
	}

	t_vton.c_iflag = (t_vton.c_iflag & (~(ICRNL|IGNCR)));
/*	t_vton.c_oflag = (t_vton.c_oflag & (~(OPOST|ONLCR)));  ~(|)
	t_vton.c_lflag = ISIG;*/
	/*t_vton.c_lflag = (t_vton.c_lflag | (t_vtoff.c_lflag & (TOSTOP)));*/
	t_vton.c_lflag = t_vtoff.c_lflag;
	t_vton.c_lflag = (t_vton.c_lflag & (~(ISIG|ICANON|ECHO)));
	t_vton.c_lflag |= (t_vtoff.c_lflag & TOSTOP);
#ifndef CNUL
#define CNUL '\0'
#endif
	/* special character processing */
/*        new.c_cc[VINTR  ] = CNUL; */
/*        new.c_cc[VQUIT  ] = CNUL; */
/*
	for ( i = 0; i <= NCCS; i++)
		t_vton.c_cc[i] = CNUL;
	t_vton.c_cc[VSTART] = t_vtoff.c_cc[VSTART];
	t_vton.c_cc[VSTOP]  = t_vtoff.c_cc[VSTOP];
*/
	t_vton.c_cc[VMIN ]  = 1; /* TODO check, may be VMIN=VTIME=0 is better ? */
	t_vton.c_cc[VTIME]  = 0;
#if 0
	t_vton.c_cc[VLNEXT]   = CNUL;
	t_vton.c_cc[VDISCARD] = CNUL;
#endif

#if 0
	vsignal(0);

#if 0
	ioctl(vtti, TCSETS, &new);
#endif
	/* tcsetattr(vtti, TCSADRAIN, &new); */
	fflush(vttout);
	/*return(tcsetattr(vtti, TCSANOW, &t_vton));*/
	return(tcsetattr(vtti, TCSADRAIN, &t_vton));
#endif
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
 * требуется скорректировать состояние клавиатуры
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
 * но требует асинхронного режима и работает не с потоком, а с дескриптором файла;
 *
 * чтение выполняется в режиме за одно обращение к этой функции - один байт, а
 * сборка в codepoint (символ UTF-8) выполняется на уровень выше, в вызывающей функции */
int
ttyinp()
{
	extern void next_j();
	int  e_saved;
	char cc[1];
	int  c;

	next_j();       /* один раз перед вводом каждого символа */
	cyrkbd();
	fflush(vttout);

	if(kb_nwt) {
		while( ttytst() ) { /* пока очередь ввода пустая */
			next_j();       /* выполнить другие асинхронные задания */
			/*
			 * в 1986 году мы с коллегами не знали что эта техника называется
			 * multitreads,
			 * а PosixThreads появились позже на несколько лет -- vsv, 2019г.
			 */
			cyrkbd();
			fflush(vttout);
		}
	}
	if (read(vtti, cc, 1) < 0) {
#if 0
		io_set(VT_ON);
#endif
		perror("\r\nread(vtti)");
		fprintf(stderr, "\r\n-- tcsetpgrp missed ?--\n");
		raise(SIGSTOP/*TTIN*/);
		/*exit(1);*/
	}
	c = cc[0] & 0377;
	return (c);
}
#endif

/*------------------------------------------------*/
/* УПРАВЛЕНИЕ РЕЖИМАМИ ТЕРМИНАЛЬНОГО ВВОДА-ВЫВОДА */
/*------------------------------------------------*/

extern int k_pad();     /* см. lin215.c */

static int oflags = 0;

int io_get(flags)
int flags;
{
	if(flags == 0)
		return(oflags);
	if(oflags & flags)
		return(1);
	return(0);
}

/*
 * main control of tty mode
 */
int io_set(flags)
register int flags;
{
	int ret;
	const char *tmps;

#ifdef VISI_DEBUG
	/* check environment 1st */
	if (debug_vt) {
		if ((tmps = getenv("VISI_DEBUG_IO")) != NULL)
			debug = atoi(tmps);
			debug_vt = 0;
	}
#endif /*VISI_DEBUG*/
	ret = oflags;
	oflags = flags;
	if(flags & IO_SAVE) {
		if (flags & VT_OFF) {
			ret = tcgetattr(vtti, &t_vtoff);
			vt_ini(); /*s_vtoff = 1;*/
		}
		if (flags & VT_ON) {
			ret = tcgetattr(vtti, &t_vton);
			s_vton = 1;
		}
	} else {
		/* переключение keypad приходится делать из-за
		 * повсеместного распространения termcap-описания для vt100, xterm,
		 * в котором клавиши со стрелками правильно работают
		 * только при включенном keypad
		 */
		if(flags & VT_ON) { /*IO_VIDEO*/
			fflush(stdout);
			/*fflush(vttout);*/
			if (s_vtoff == 0)
				{ vt_ini(); /*s_vtoff = 1;*/ }
			vt_on();
			k_pad(1);
			if(flags & IO_WAIT)
						kb_nwt = 0;
			if(flags & IO_NOWAIT)
						kb_nwt = 1;
		}
		if(flags & VT_OFF) { /*IO_TTYPE*/
			k_pad(0);
			fflush(vttout);
			/*fflush(stdout);*/
			vt_off();
		}
	}
	return(ret);
}

#endif  /* USE_TERMIOS */

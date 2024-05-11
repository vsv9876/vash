/*
 * Размеры таблиц для интерпретации команд.
 */
#ifndef TINYSMALL

#define PCMAX   600
#define KT2MAX  100
#define KT1MAX  200
#define BUFSMAX 16000

#else

#define PCMAX   200
#define KT2MAX  30
#define KT1MAX  60
#define BUFSMAX 2000

#endif /* TINYSMALL */

/*
 * Структуры данных для интерпретатора клавишных команд.
 */

/* таблица шаблон/действие */
typedef struct {
	char *pc_pat;   /* шаблон условия */
	char *pc_cmd;   /* выполняемая команда */
} PATCMD;

/* таблица быстрого просмотра */
typedef struct {
	char   *kt_key; /* ключ поиска */
	PATCMD *kt_tab; /* адрес таблицы интерпретации */
	int     kt_ib;  /* индекс нач. */
	int     kt_ie;  /* индекс конец */
} KEYTAB;

/* таблица имен встроенных функций */
typedef struct {
	char   *ft_name;        /* строка с именем */
	int    (*ft_fun)();    /* указатель на функцию */
} FUNTAB;

extern  char *tstat1();
extern  char *tstat2();

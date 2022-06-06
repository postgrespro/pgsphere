/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20200330

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0

#ifndef yyparse
#define yyparse    sphere_yyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      sphere_yylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    sphere_yyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     sphere_yychar
#endif /* yychar */

#ifndef yyval
#define yyval      sphere_yyval
#endif /* yyval */

#ifndef yylval
#define yylval     sphere_yylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    sphere_yydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    sphere_yynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  sphere_yyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      sphere_yylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      sphere_yylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   sphere_yydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    sphere_yydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   sphere_yysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   sphere_yyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   sphere_yygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    sphere_yytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    sphere_yycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     sphere_yyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     sphere_yyrule
#endif /* yyrule */
#define YYPREFIX "sphere_yy"

#define YYPURE 0

#line 2 "sparse.y"
#include <stdio.h>
#include <string.h>
#include "postgres.h"
#include "utils/elog.h"
#include "sbuffer.h"

#define YYMALLOC palloc
#define YYFREE   pfree

void sphere_yyerror(const char *str)
{
	reset_buffer();
	elog(ERROR, "parsing: %s", str);
}

static double human2dec(double d, double m, double s)
{
	if (m < 0 || m >= 60 || s < 0 || s >= 60)
	{
		char err[255];
		sprintf(err, "minutes or seconds invalid (min:%f sec:%f)", m, s);
		sphere_yyerror(err);
		return 0;
	}
	else
	{
		if (d < 0)
			return (-s / 3600.0 - m / 60.0) + d;
		else
			return (s / 3600.0 + m / 60.0) + d;
	}
}

#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 41 "sparse.y"
typedef union {
	int		i;
	double	d;
	char	c[3];
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 148 "sparse.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define HOUR 257
#define DEG 258
#define MIN 259
#define SEC 260
#define COMMA 261
#define OPENCIRC 262
#define CLOSECIRC 263
#define OPENPOINT 264
#define CLOSEPOINT 265
#define OPENARR 266
#define CLOSEARR 267
#define SIGN 268
#define INT 269
#define FLOAT 270
#define EULERAXIS 271
#define YYERRCODE 256
typedef int YYINT;
static const YYINT sphere_yylhs[] = {                    -1,
    0,    0,    9,    9,    9,    9,    9,    9,    9,    1,
    1,    3,    3,    3,    3,    3,    3,    3,    3,    5,
    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
    4,    4,    2,    2,    6,   10,   12,   12,   11,    7,
    7,    8,   13,   14,   14,
};
static const YYINT sphere_yylen[] = {                     2,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    3,    4,    4,    5,    6,    1,
    2,    2,    3,    4,    4,    5,    6,    3,    5,    6,
    1,    2,    1,    2,    5,    5,    5,    7,    5,    2,
    3,    4,   11,    5,    3,
};
static const YYINT sphere_yydefred[] = {                  0,
    0,    0,    0,    0,    0,    0,    0,   20,    0,   31,
    0,    7,    2,    4,    5,    6,    8,    9,    0,    0,
    0,    0,    0,    0,    0,    0,   32,    0,    0,   21,
    0,    0,    0,    0,    0,   12,    0,    0,    0,    0,
    0,   40,    0,    0,   10,   28,    0,    0,   23,    0,
   45,    0,    0,   13,    0,    0,    0,    0,    0,    0,
    0,   33,    0,    0,   41,   42,    0,    0,   24,    0,
    0,    0,    0,   15,    0,   36,   34,    0,    0,   35,
   44,   39,   11,    0,    0,    0,    0,   16,    0,    0,
    0,    0,   30,   27,    0,    0,    0,    0,    0,   38,
   19,    0,    0,    0,    0,    0,   43,
};
static const YYINT sphere_yydgoto[] = {                   7,
    8,   61,   62,   50,   10,   11,   26,   12,   13,   14,
   15,   16,   17,   18,
};
static const YYINT sphere_yysindex[] = {               -183,
 -210, -248, -228, -152, -138, -229,    0,    0, -217,    0,
 -215,    0,    0,    0,    0,    0,    0,    0, -251, -148,
 -212, -197, -172, -187, -166, -228,    0, -146, -144,    0,
 -251, -228, -155, -121, -120,    0, -151, -148, -157, -228,
 -122,    0, -256, -119,    0,    0, -118, -117,    0, -116,
    0, -154, -142,    0, -148, -159, -140, -126, -115,    0,
 -114,    0, -113, -152,    0,    0, -136, -136,    0, -251,
 -148, -112, -111,    0, -123,    0,    0, -134,    0,    0,
    0,    0,    0, -110, -107, -106, -136,    0, -105, -102,
 -101,    0,    0,    0, -125, -100, -228, -136,    0,    0,
    0,  -99,  -97, -154,    0, -109,    0,
};
static const YYINT sphere_yyrindex[] = {                149,
    0,    0,    0,    0,    1,    2,    0,    0,    0,    0,
  159,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    3,    0,
    0,    0,    0, -235, -206,    0,    0,    0,    0,    0,
    0,    0,    0,    1,    0,    0,    1,    2,    0,    0,
    0,    0, -196,    0,    0,    0,    0, -234, -226, -223,
    0,    0,    0,    0,    0,    0,    0,    4,    0,    0,
    0, -235, -206,    0,    0,    0,    0, -220, -181,    0,
    0,    0,    0,    9,   10,    7, -193,    0,    0, -234,
 -226, -165,    0,    0,    0, -173,    0, -164, -163,    0,
    0,    0, -158,    0, -156,    0,    0,
};
static const YYINT sphere_yygindex[] = {                  0,
   -5,   57,   20,    6,   -4,   11,    0,    0,    0,    0,
    0,  162,    0,    0,
};
#define YYTABLESIZE 275
static const YYINT sphere_yytable[] = {                  27,
   11,   10,   22,   25,   65,    9,   37,   22,   29,   26,
   66,   21,   23,   25,   36,   19,    4,    5,    6,    4,
    5,    6,   46,   49,   33,   11,   11,   11,   30,   11,
   11,   11,   36,   60,   10,   19,   43,   20,   10,   37,
   22,   12,   51,   31,   14,   32,   36,   74,   38,   36,
   63,   60,   27,   19,   10,   20,   10,   56,   10,   82,
   10,   84,   85,   39,   14,   36,   14,   17,   14,   17,
   14,   17,   92,   17,   75,   86,   77,   41,    1,   21,
    2,   96,    3,   13,    4,    5,    6,   18,   40,   18,
   77,   18,  103,   18,   42,   23,   25,   24,   36,   15,
   17,   16,   26,   76,   27,   52,   18,  102,   19,   55,
   57,   58,   59,   71,   34,   35,    5,    6,   28,   29,
   34,   35,   44,   45,   47,   48,   72,   73,   58,   59,
   28,   78,   83,   45,   90,   91,   53,   54,   64,   67,
   68,   69,   79,   89,   70,  100,   87,   88,    1,   93,
   80,   81,   94,  107,   95,   97,   98,   99,    3,  101,
  106,  104,  105,   24,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   11,   10,   22,   25,   11,   10,   22,   25,   29,
   26,   37,    0,   29,   26,
};
static const YYINT sphere_yycheck[] = {                   4,
    0,    0,    0,    0,  261,    0,    0,    2,    0,    0,
  267,    1,    2,    3,   20,  264,  268,  269,  270,  268,
  269,  270,   28,   29,   19,  261,  261,  263,  258,  265,
  265,  267,   38,   39,  261,  264,   26,  261,  265,   20,
  261,  265,   32,  261,  265,  261,   52,   53,  261,   55,
   40,   57,   57,  264,  261,  266,  263,   38,  265,   64,
  267,   67,   68,  261,  261,   71,  263,  261,  265,  263,
  267,  265,   78,  267,   55,   70,   57,  265,  262,  261,
  264,   87,  266,  265,  268,  269,  270,  261,  261,  263,
   71,  265,   98,  267,  261,  261,  261,  261,  104,  265,
  265,  265,  261,  263,  261,  261,  265,   97,  265,  261,
  268,  269,  270,  268,  269,  270,  269,  270,  257,  258,
  269,  270,  269,  270,  269,  270,  269,  270,  269,  270,
  257,  258,  269,  270,  269,  270,  258,  258,  261,  259,
  259,  259,  258,  267,  261,  271,  259,  259,    0,  260,
  265,  265,  260,  263,  261,  261,  259,  259,    0,  260,
  104,  261,  260,    2,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  261,  261,  261,  261,  265,  265,  265,  265,  261,
  261,  265,   -1,  265,  265,
};
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 271
#define YYUNDFTOKEN 288
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const sphere_yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"HOUR","DEG","MIN","SEC","COMMA",
"OPENCIRC","CLOSECIRC","OPENPOINT","CLOSEPOINT","OPENARR","CLOSEARR","SIGN",
"INT","FLOAT","EULERAXIS",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const sphere_yyrule[] = {
"$accept : commands",
"commands :",
"commands : command",
"command : spherepoint",
"command : spherecircle",
"command : sphereline",
"command : eulertrans",
"command : spherepath",
"command : sphereellipse",
"command : spherebox",
"number : FLOAT",
"number : INT",
"angle_lat_us : number",
"angle_lat_us : FLOAT DEG",
"angle_lat_us : INT DEG",
"angle_lat_us : INT DEG number",
"angle_lat_us : INT DEG FLOAT MIN",
"angle_lat_us : INT DEG INT MIN",
"angle_lat_us : INT DEG INT MIN number",
"angle_lat_us : INT DEG INT MIN number SEC",
"angle_long_us : number",
"angle_long_us : FLOAT DEG",
"angle_long_us : INT DEG",
"angle_long_us : INT DEG number",
"angle_long_us : INT DEG FLOAT MIN",
"angle_long_us : INT DEG INT MIN",
"angle_long_us : INT DEG INT MIN number",
"angle_long_us : INT DEG INT MIN number SEC",
"angle_long_us : INT HOUR number",
"angle_long_us : INT HOUR INT MIN number",
"angle_long_us : INT HOUR INT MIN number SEC",
"angle_long : angle_long_us",
"angle_long : SIGN angle_long_us",
"angle_lat : angle_lat_us",
"angle_lat : SIGN angle_lat_us",
"spherepoint : OPENPOINT angle_long COMMA angle_lat CLOSEPOINT",
"spherecircle : OPENCIRC spherepoint COMMA angle_lat_us CLOSECIRC",
"eulertrans : angle_long COMMA angle_long COMMA angle_long",
"eulertrans : angle_long COMMA angle_long COMMA angle_long COMMA EULERAXIS",
"sphereline : OPENPOINT eulertrans CLOSEPOINT COMMA angle_long_us",
"spherepointlist : spherepoint COMMA",
"spherepointlist : spherepointlist spherepoint COMMA",
"spherepath : OPENARR spherepointlist spherepoint CLOSEARR",
"sphereellipse : OPENCIRC OPENARR angle_lat_us COMMA angle_lat_us CLOSEARR COMMA spherepoint COMMA angle_lat CLOSECIRC",
"spherebox : OPENPOINT spherepoint COMMA spherepoint CLOSEPOINT",
"spherebox : spherepoint COMMA spherepoint",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yym = 0;
    yyn = 0;
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 3:
#line 70 "sparse.y"
	{ set_spheretype( STYPE_POINT   ); }
break;
case 4:
#line 71 "sparse.y"
	{ set_spheretype( STYPE_CIRCLE  ); }
break;
case 5:
#line 72 "sparse.y"
	{ set_spheretype( STYPE_LINE    ); }
break;
case 6:
#line 73 "sparse.y"
	{ set_spheretype( STYPE_EULER   ); }
break;
case 7:
#line 74 "sparse.y"
	{ set_spheretype( STYPE_PATH    ); }
break;
case 8:
#line 75 "sparse.y"
	{ set_spheretype( STYPE_ELLIPSE ); }
break;
case 9:
#line 76 "sparse.y"
	{ set_spheretype( STYPE_BOX     ); }
break;
case 10:
#line 81 "sparse.y"
	{ yyval.d = yystack.l_mark[0].d; }
break;
case 11:
#line 82 "sparse.y"
	{ yyval.d = yystack.l_mark[0].i; }
break;
case 12:
#line 87 "sparse.y"
	{ yyval.i = set_angle(0, yystack.l_mark[0].d ); }
break;
case 13:
#line 88 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-1].d, 0, 0) ); }
break;
case 14:
#line 89 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-1].i, 0, 0) ); }
break;
case 15:
#line 90 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-2].i, yystack.l_mark[0].d, 0) ); }
break;
case 16:
#line 91 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-3].i, yystack.l_mark[-1].d, 0) ); }
break;
case 17:
#line 92 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-3].i, yystack.l_mark[-1].i, 0) ); }
break;
case 18:
#line 93 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-4].i, yystack.l_mark[-2].i, yystack.l_mark[0].d) ); }
break;
case 19:
#line 94 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-5].i, yystack.l_mark[-3].i, yystack.l_mark[-1].d) ); }
break;
case 20:
#line 99 "sparse.y"
	{ yyval.i = set_angle(0, yystack.l_mark[0].d); }
break;
case 21:
#line 100 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-1].d, 0, 0)); }
break;
case 22:
#line 101 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-1].i, 0, 0)); }
break;
case 23:
#line 102 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-2].i, yystack.l_mark[0].d, 0)); }
break;
case 24:
#line 103 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-3].i, yystack.l_mark[-1].d, 0)); }
break;
case 25:
#line 104 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-3].i, yystack.l_mark[-1].i, 0)); }
break;
case 26:
#line 105 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-4].i, yystack.l_mark[-2].i, yystack.l_mark[0].d)); }
break;
case 27:
#line 106 "sparse.y"
	{ yyval.i = set_angle(1, human2dec(yystack.l_mark[-5].i, yystack.l_mark[-3].i, yystack.l_mark[-1].d)); }
break;
case 28:
#line 107 "sparse.y"
	{ yyval.i = set_angle(1, 15 * human2dec(yystack.l_mark[-2].i, yystack.l_mark[0].d, 0)); }
break;
case 29:
#line 108 "sparse.y"
	{ yyval.i = set_angle(1, 15 * human2dec(yystack.l_mark[-4].i, yystack.l_mark[-2].i, yystack.l_mark[0].d)); }
break;
case 30:
#line 109 "sparse.y"
	{ yyval.i = set_angle(1, 15 * human2dec(yystack.l_mark[-5].i, yystack.l_mark[-3].i, yystack.l_mark[-1].d)); }
break;
case 31:
#line 114 "sparse.y"
	{ yyval.i = set_angle_sign(yystack.l_mark[0].i, 1); }
break;
case 32:
#line 115 "sparse.y"
	{ yyval.i = set_angle_sign(yystack.l_mark[0].i, yystack.l_mark[-1].i); }
break;
case 33:
#line 120 "sparse.y"
	{ yyval.i = set_angle_sign(yystack.l_mark[0].i, 1); }
break;
case 34:
#line 121 "sparse.y"
	{ yyval.i = set_angle_sign(yystack.l_mark[0].i, yystack.l_mark[-1].i); }
break;
case 35:
#line 128 "sparse.y"
	{
			yyval.i = set_point(yystack.l_mark[-3].i, yystack.l_mark[-1].i);
		}
break;
case 36:
#line 136 "sparse.y"
	{
			set_circle(yystack.l_mark[-3].i, yystack.l_mark[-1].i);
		}
break;
case 37:
#line 144 "sparse.y"
	{
			set_euler(yystack.l_mark[-4].i, yystack.l_mark[-2].i, yystack.l_mark[0].i, "ZXZ");
		}
break;
case 38:
#line 148 "sparse.y"
	{
			set_euler(yystack.l_mark[-6].i, yystack.l_mark[-4].i, yystack.l_mark[-2].i, yystack.l_mark[0].c);
		}
break;
case 39:
#line 156 "sparse.y"
	{
			set_line (yystack.l_mark[0].i);
		}
break;
case 42:
#line 168 "sparse.y"
	{ }
break;
case 43:
#line 174 "sparse.y"
	{
			set_ellipse(yystack.l_mark[-8].i, yystack.l_mark[-6].i, yystack.l_mark[-3].i, yystack.l_mark[-1].i);
		}
break;
case 44:
#line 181 "sparse.y"
	{ }
break;
case 45:
#line 182 "sparse.y"
	{ }
break;
#line 800 "sparse.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}

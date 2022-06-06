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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	int		i;
	double	d;
	char	c[3];
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE sphere_yylval;

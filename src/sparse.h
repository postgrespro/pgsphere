/* A Bison parser, made by GNU Bison 3.4.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_SPHERE_YY_SPARSE_H_INCLUDED
# define YY_SPHERE_YY_SPARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int sphere_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    HOUR = 258,
    DEG = 259,
    MIN = 260,
    SEC = 261,
    COMMA = 262,
    OPENCIRC = 263,
    CLOSECIRC = 264,
    OPENPOINT = 265,
    CLOSEPOINT = 266,
    OPENARR = 267,
    CLOSEARR = 268,
    SIGN = 269,
    INT = 270,
    FLOAT = 271,
    EULERAXIS = 272
  };
#endif
/* Tokens.  */
#define HOUR 258
#define DEG 259
#define MIN 260
#define SEC 261
#define COMMA 262
#define OPENCIRC 263
#define CLOSECIRC 264
#define OPENPOINT 265
#define CLOSEPOINT 266
#define OPENARR 267
#define CLOSEARR 268
#define SIGN 269
#define INT 270
#define FLOAT 271
#define EULERAXIS 272

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 41 "sparse.y"

	int		i;
	double	d;
	char	c[3];

#line 97 "sparse.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE sphere_yylval;

int sphere_yyparse (void);

#endif /* !YY_SPHERE_YY_SPARSE_H_INCLUDED  */

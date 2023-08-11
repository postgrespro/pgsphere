\set ECHO none
SELECT set_sphere_output_precision(8);
\set ECHO all
SET extra_float_digits TO -3;

-- checking spherical line operators

SELECT sline ( spoint '(0, 90d)', spoint '(0, -89d)' )   =
       sline ( spoint '(0, 90d)', spoint '(0, -89d)' )  ;
SELECT sline ( spoint '(0,  90d)', spoint '(0, -89d)' )   <>
       sline ( spoint '(0, -89d)', spoint '(0,  90d)' ) ;



SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(5d,  5d)', spoint '(5d, -5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(10d,  5d)', spoint '(10d, -5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(15d,  5d)', spoint '(15d, -5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(10d,  0d)', spoint '(10d, -5d)' ) ;



SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(5d,  -5d)', spoint '(5d, 5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(10d, -5d)', spoint '(10d, 5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(15d, -5d)', spoint '(15d, 5d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(10d, 0d)' )   #
       sline ( spoint '(10d,  0d)', spoint '(10d, 5d)' ) ;
       
       
-- check small lines


SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.0000005d,  0.0000005d)', spoint '(0.0000005d, -0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.000001d,  0.0000005d)', spoint '(0.000001d, -0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.0000015d,  0.0000005d)', spoint '(0.0000015d, -0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.000001d,  0d)', spoint '(0.000001d, -0.0000005d)' ) ;



SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.0000005d,  -0.0000005d)', spoint '(0.0000005d, 0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.000001d, -0.0000005d)', spoint '(0.000001d, 0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.0000015d, -0.0000005d)', spoint '(0.0000015d, 0.0000005d)' ) ;

SELECT sline ( spoint '(0,   0d)', spoint '(0.000001d, 0d)' )   #
       sline ( spoint '(0.000001d,  0d)', spoint '(0.000001d, 0.0000005d)' ) ;


-- checking the distance between a line and a point


SELECT sline '( 90d, 0d, 0d, XYZ ), 40d ' <-> spoint '( 0d, 90d )';

SELECT sline '( 90d, 0d, 0d, XYZ ), 40d ' <-> spoint '( 0d, 90d )'   =
       spoint '( 0d, 90d )' <-> sline '( 90d, 0d, 0d, XYZ ), 40d ';

SELECT sline '( 0d, 0d, 0d, XYZ ), 0d ' <-> spoint '( 0d, 90d )';

SELECT sline '( 0d, 0d, 0d, XYZ ), 0d ' <-> spoint '( 0d, 0d )';

SELECT sline '( 0d, 0d, 0d, XYZ ), 30d ' <-> spoint '( 0d, 30d )';

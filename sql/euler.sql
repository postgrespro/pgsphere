\set ECHO none
SELECT set_sphere_output_precision(8);
SET extra_float_digits TO -3;
\set ECHO all

-- checking Euler transformation operators

SELECT strans '-10d,0d,10d,ZZZ'  = '-10d,0d,10d,XXX' ;
SELECT strans '-40d,0d,40d,ZZZ' <> '-40d,0d,40d,XXX' ;

SELECT strans ( 20.0*pi()/180.0, -270.0*pi()/180.0, 70.5*pi()/180.0, 'XZY' );

SELECT theta( strans ( 20.0*pi()/180.0, -270.0*pi()/180.0, 70.5*pi()/180.0, 'XZY' ) );

SELECT psi( strans ( 20.0*pi()/180.0, -270.0*pi()/180.0, 70.5*pi()/180.0, 'XZY' ) );

SELECT phi( strans ( 20.0*pi()/180.0, -270.0*pi()/180.0, 70.5*pi()/180.0, 'XZY' ) );

SELECT theta( strans( '20d, 30d, 40d, XZY' ) );

SELECT psi( strans( '20d, 30d, 40d, XZY' ) );

SELECT phi( strans( '20d, 30d, 40d, XZY' ) );

SELECT strans( '2d 20m, 10d, 0' );

SELECT theta( strans( '2d 20m, 10d, 0' ) );

SELECT psi( strans( '2d 20m, 10d, 0' ) );

SELECT phi( strans( '2d 20m, 10d, 0' ) );

SELECT strans ( '10d, 90d, 270d, ZXZ' );

SELECT theta( strans ( '10d, 90d, 270d, ZXZ' ) );

SELECT psi( strans ( '10d, 90d, 270d, ZXZ' ) );

SELECT phi( strans ( '10d, 90d, 270d, ZXZ' ) );

SELECT - strans ( '20d, 50d, 80d, XYZ' );

SELECT theta( - strans ( '20d, 50d, 80d, XYZ' ) );

SELECT psi( - strans ( '20d, 50d, 80d, XYZ' ) );

SELECT phi( - strans ( '20d, 50d, 80d, XYZ' ) );

SELECT strans( '90d, 60d, 30d' );

SELECT theta( strans( '90d, 60d, 30d' ) );

SELECT psi( strans( '90d, 60d, 30d' ) );

SELECT phi( strans( '90d, 60d, 30d' ) );

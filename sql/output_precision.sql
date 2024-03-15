--
-- Test default and custom output precisions for double values.
--

SELECT set_sphere_output( 'RAD' );

--
-- Check default precision
--
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

--
-- Check option extra_float_digits
--
SET extra_float_digits TO -6;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO -2;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO 0;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO 1;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO 2;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO 3;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO 6;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

--
-- Check compatibility behaviour
--
SELECT set_sphere_output_precision(10);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(12);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(15);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(17);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(20);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(0);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SELECT set_sphere_output_precision(-3);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

--
-- Check extra_float_digits after set_sphere_output_precision.
-- The change of extra_float_digits should not affect the precision of pgsphere
-- output because set_sphere_output_precision enables compatibility mode.
--
SELECT set_sphere_output_precision(10);
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO -6;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO -10;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

--
-- Check reset_sphere_output_precision.
-- It should disable compatibility mode - extra_float_digits should work.
--
SELECT reset_sphere_output_precision();
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

SET extra_float_digits TO -6;
SELECT '( 1h 2m 30s , +1d 2m 30s)'::spoint;

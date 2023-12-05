\set ECHO none
SELECT set_sphere_output_precision(8);
SET extra_float_digits = 0;
\set ECHO all

-- checking polygon operators

\set poly 'spoly \'{(0.1,0),(0.2,0),(0.2,0.1),(0.3,0.1),(0.3,-0.1),(0.4,-0.1),(0.5,0.1),(0.4,0.2),(0.1,0.2)}\''
SELECT spoint   '(0.15,0.10)' @ :poly;                  -- point inside polygon
SELECT spoint   '(0.20,0.00)' @ :poly;                  -- point contained polygon
SELECT spoint   '(0.10,0.10)' @ :poly;                  -- point contained polygon
SELECT spoint   '(0.25,0.50)' @ :poly;                  -- point outside polygon
SELECT spoint   '(0.25,0.00)' @ :poly;                  -- point outside polygon
SELECT scircle  '<(0.15,0.10),0.03>' @  :poly;          -- circle inside polygon
SELECT scircle  '<(0.20,0.00),0.00>' @  :poly;          -- circle contained polygon
SELECT scircle  '<(0.20,0.30),0.05>' @  :poly;          -- circle outside  polygon
SELECT scircle  '<(0.25,0.00),0.05>' @  :poly;          -- circle overlaps polygon
SELECT scircle  '<(0.25,0.00),0.10>' @  :poly;          -- circle overlaps polygon
SELECT scircle  '<(0.15,0.10),0.03>' && :poly;          -- circle inside polygon
SELECT scircle  '<(0.20,0.00),0.00>' && :poly;          -- circle contained polygon
SELECT scircle  '<(0.20,0.30),0.05>' && :poly;          -- circle outside  polygon
SELECT scircle  '<(0.25,0.00),0.05>' && :poly;          -- circle overlaps polygon
SELECT scircle  '<(0.25,0.00),0.10>' && :poly;          -- circle overlaps polygon
SELECT sline ( spoint '(0.00, 0.00)', spoint '(0.10,0.20)' ) @  :poly;  -- line touches polygon
SELECT sline ( spoint '(0.00, 0.10)', spoint '(0.10,0.10)' ) @  :poly;  -- line touches polygon
SELECT sline ( spoint '(0.50, 0.00)', spoint '(0.50,0.20)' ) @  :poly;  -- line touches polygon
SELECT sline ( spoint '(0.10, 0.20)', spoint '(0.20,0.00)' ) @  :poly;  -- line touches and inside polygon
SELECT sline ( spoint '(0.45,-0.20)', spoint '(0.45,0.20)' ) @  :poly;  -- line overlaps polygon
SELECT sline ( spoint '(0.45, 0.10)', spoint '(0.45,0.20)' ) @  :poly;  -- line overlaps polygon
SELECT sline ( spoint '(0.24, 0.17)', spoint '(0.25,0.14)' ) @  :poly;  -- line inside  polygon
SELECT sline ( spoint '(0.00, 0.00)', spoint '(0.10,0.20)' ) && :poly;  -- line touches polygon
SELECT sline ( spoint '(0.00, 0.10)', spoint '(0.10,0.10)' ) && :poly;  -- line touches polygon
SELECT sline ( spoint '(0.50, 0.00)', spoint '(0.50,0.20)' ) && :poly;  -- line touches polygon
SELECT sline ( spoint '(0.10, 0.20)', spoint '(0.20,0.00)' ) && :poly;  -- line touches and inside polygon
SELECT sline ( spoint '(0.45,-0.20)', spoint '(0.45,0.20)' ) && :poly;  -- line overlaps polygon
SELECT sline ( spoint '(0.45, 0.10)', spoint '(0.45,0.20)' ) && :poly;  -- line overlaps polygon
SELECT sline ( spoint '(0.24, 0.17)', spoint '(0.25,0.14)' ) && :poly;  -- line inside  polygon
\unset poly

\set poly1 'spoly \'{(0,0),(1,0),(0,1)}\''
\set poly2 'spoly \'{(1,0),(0,0),(0,1)}\''
\set poly3 'spoly \'{(0,1),(0,0),(1,0)}\''
\set poly4 'spoly \'{(0.1,0.9),(0.1,0.1),(0.9,0.1)}\''
\set poly5 'spoly \'{(0.2,0.0),(1.2,0.0),(0.2,1)}\''

SELECT :poly1  = :poly2;
SELECT :poly2  = :poly3;
SELECT :poly3  = :poly1;
SELECT :poly1 && :poly2;
SELECT :poly2 && :poly3;
SELECT :poly3 && :poly1;
SELECT :poly1  @ :poly2;
SELECT :poly2  @ :poly3;
SELECT :poly3  @ :poly1;
SELECT :poly1  @ :poly4;
SELECT :poly4  @ :poly1;
SELECT :poly1 && :poly4;
SELECT :poly4 && :poly1;
SELECT :poly1  @ :poly5;
SELECT :poly5  @ :poly1;
SELECT :poly1 && :poly5;
SELECT :poly5 && :poly1;

\unset poly1
\unset poly2
\unset poly3
\unset poly4
\unset poly5


-- From testsuite/poly_test.sql

SELECT set_sphere_output('DEG');

SELECT spoly '{(10d,0d),(10d,1d),(15d,0d)}';

SELECT spoly '{(359d,0d),(359d,1d),(4d,0d)}';

SELECT spoly '{(10d,0d),(10d,1d),(15d,0d)}';

SELECT spoly(ARRAY[0.017453292519943295, 0.03490658503988659, 0.05235987755982988, 0.06981317007977318, 0.08726646259971647, 0.10471975511965977]);

SELECT spoly(ARRAY[0.17453292519943295, 0.0, 0.17453292519943295, 0.017453292519943295, 0.2617993877991494, 0.0]);

SELECT spoly_deg(ARRAY[1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);

SELECT spoly_deg(ARRAY[10.0, 0.0, 10.0, 1.0, 15.0, 0.0]);

--- Constructors

SELECT spoly(NULL::spoint[]);

SELECT spoly(ARRAY[]::spoint[]);

SELECT spoly(ARRAY[spoint_deg(0, 0)]);

SELECT spoly(ARRAY[spoint_deg(0, 0), spoint_deg(10, 0)]);

SELECT spoly(ARRAY[spoint_deg(0, 0), spoint_deg(10, 0), spoint_deg(10, 10)]);

SELECT spoly(ARRAY[spoint_deg(0, 0), spoint_deg(10, 0), spoint_deg(10, 10), spoint_deg(0, 10)]);

--- incorrect input -----

SELECT spoly '{(10d,0d),(10d,1d)}';

SELECT spoly(ARRAY[1.0, 2.0, 3.0, 4.0, 5.0]);

SELECT spoly(ARRAY[1.0, 2.0, 3.0, NULL, 5.0, 6.0]);

SELECT spoly(ARRAY[]::float8[]);

SELECT spoly(NULL::float8[]);

SELECT spoly_deg(ARRAY[1.0, 2.0, 3.0, 4.0, 5.0]);

SELECT spoly_deg(ARRAY[1.0, 2.0, 3.0, NULL, 5.0, 6.0]);

SELECT spoly_deg(ARRAY[]::float8[]);

SELECT spoly_deg(NULL::float8[]);

--- self-crossing input -----

SELECT spoly '{(0d,0d),(10d,10d),(0d,10d),(10d,0d)}';

--- degenerate polygons -----

SELECT spoly '{(0d,1d),(0d,2d),(0d,3d)}';

SELECT spoly '{(1d,0d),(2d,0d),(3d,0d)}';

--- functions

SELECT npoints( spoly '{(10d,0d),(10d,1d),(15d,0d)}');

SELECT npoints( spoly '{(10d,0d),(10d,1d),(15d,0d),(5d,-5d)}');

--SELECT npoints( spoly '{(0d,0d),(0d,90d),(15d,90d),(15d,0d)}');

SELECT area(spoly '{(0d,0d),(0d,90d),(1,0d)}');

SELECT area(spoly '{(0d,0d),(0d,90d),(90d,0d)}')/(4.0*pi());

--- operations

--- = operator

--- should be true

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' = spoly '{(1d,1d),(2d,1d),(1d,0d)}';

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' = spoly '{(2d,1d),(1d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' = spoly '{(1d,0d),(0d,0d),(0d,1d),(1d,1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' = spoly '{(0d,0d),(1d,0d),(1d,1d),(0d,1d)}';

--- should be false

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' = spoly '{(1d,1d),(3d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' = spoly '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}';

--- <> operator

--- should be false

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' <> spoly '{(1d,1d),(2d,1d),(1d,0d)}';

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' <> spoly '{(2d,1d),(1d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' <> spoly '{(1d,0d),(0d,0d),(0d,1d),(1d,1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' <> spoly '{(0d,0d),(1d,0d),(1d,1d),(0d,1d)}';

--- should be true

SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' <> spoly '{(1d,1d),(3d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' <> spoly '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}';

--- spoint @ spoly

--- should be true
SELECT spoly '{(1d,0d),(1d,1d),(2d,1d)}' <> spoly '{(1d,1d),(3d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' <> spoly '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}';

--- spoint @ spoly

--- should be true

SELECT '(0.5d,0.5d)'::spoint @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '(0d,0.5d)'::spoint @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '(0d,0d)'::spoint @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '(0.5d,0.5d)'::spoint @ spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '(0d,89.9d)'::spoint @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT '(0d,90d)'::spoint @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT '(0d,-89.9d)'::spoint @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT '(0d,-90d)'::spoint @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

--- should be false

SELECT '(0.1d,0.5d)'::spoint @ spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '(45d,-89d)'::spoint @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT '(0d,1d)'::spoint @ spoly '{(0d,0d),(1d,1d),(1d,0d)}';

--- spoly ~ spoint

--- should be true

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '(0.5d,0.5d)'::spoint;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '(0d,0.5d)'::spoint;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '(0d,0d)'::spoint;

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ~ '(0.5d,0.5d)'::spoint;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ '(0d,89.9d)'::spoint;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ '(0d,90d)'::spoint;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '(0d,-89.9d)'::spoint;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '(0d,-90d)'::spoint;

--- should be false

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ~ '(0.1d,0.5d)'::spoint;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '(45d,-89d)'::spoint;

SELECT  spoly '{(0d,0d),(1d,1d),(1d,0d)}' ~ '(0d,1d)'::spoint;

--- scircle @ spoly

--- should be true

SELECT '<(0.5d,0.5d),0.1d>'::scircle @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(0d,89.9d),0.1d>'::scircle @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT '<(0d,90d),0.1d>'::scircle @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT '<(0d,-89.9d),0.1d>'::scircle @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT '<(0d,-90d),0.1d>'::scircle @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

--- should be false

SELECT '<(0.1d,0.5d),0.1d>'::scircle @ spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(45d,-89d),0.1d>'::scircle @ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT '<(0d,1d),0.1d>'::scircle @ spoly '{(0d,0d),(1d,1d),(1d,0d)}';

SELECT '<(0d,0.5d),0.1d>'::scircle @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(0d,0d),0.1d>'::scircle @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(0.5d,0.5d),0.1d>'::scircle @ spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

--- spoly ~ scircle

--- should be true

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0.5d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ '<(0d,89.9d),0.1d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ '<(0d,90d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '<(0d,-89.9d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '<(0d,-90d),0.1d>'::scircle;

--- should be false

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0.1d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ~ '<(45d,-89d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(1d,1d),(1d,0d)}' ~ '<(0d,1d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0d,0d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0.1d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0.1d,0.1d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ~ '<(0.6d,0.5d),0.1d>'::scircle;

--- spoly @ scircle

--- should be true

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' @ '<(0d,0d),2.0d>'::scircle;

SELECT spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' @ '<(0d,0d),1.0d>'::scircle;

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' @ '<(0d,90d),1.0d>'::scircle;

SELECT spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' @ '<(180d,-90d),1.0d>'::scircle;

SELECT spoly '{(0d,0d),(0d,1d),(1d,0d)}' @ '<(0d,0d),1.0d>'::scircle;

--- should be false

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' @ '<(0d,0d),1.0d>'::scircle;

SELECT spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' @ '<(0d,0d),0.99d>'::scircle;

SELECT spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' @ '<(60d,0d),0.99d>'::scircle;

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,88d)}' @ '<(0d,90d),1.0d>'::scircle;

SELECT spoly '{(0d,-87d),(90d,-87d),(180d,-87d),(270d,-87d)}' @ '<(180d,-90d),1.0d>'::scircle;

SELECT spoly '{(0d,0d),(0d,1d),(2d,0d)}' @ '<(0d,0d),1.0d>'::scircle;

--- scircle ~ spoly

--- should be true

SELECT '<(0d,0d),2.0d>'::scircle ~ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(0d,0d),1.0d>'::scircle ~ spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}';

SELECT '<(0d,90d),1.0d>'::scircle ~ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT '<(180d,-90d),1.0d>'::scircle ~ spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT '<(0d,0d),1.0d>'::scircle ~ spoly '{(0d,0d),(0d,1d),(1d,0d)}';

--- should be false

SELECT '<(0d,0d),1.0d>'::scircle ~ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT '<(0d,0d),0.99d>'::scircle ~ spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}';

SELECT '<(60d,0d),0.99d>'::scircle ~ spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}';

SELECT '<(0d,90d),1.0d>'::scircle ~ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,88d)}';

SELECT '<(180d,-90d),1.0d>'::scircle ~ spoly '{(0d,-87d),(90d,-87d),(180d,-87d),(270d,-87d)}';

SELECT '<(0d,0d),1.0d>'::scircle ~ spoly '{(0d,0d),(0d,1d),(2d,0d)}';

--- scircle && spoly

--- should be true

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' && '<(0.5d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && '<(0d,89.9d),0.1d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && '<(0d,90d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' && '<(0d,-89.9d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' && '<(0d,-90d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' && '<(0d,0d),2.0d>'::scircle;

SELECT  spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' && '<(0d,0d),1.0d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && '<(0d,90d),1.0d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' && '<(180d,-90d),1.0d>'::scircle;

SELECT  spoly '{(0d,0d),(0d,1d),(1d,0d)}' && '<(0d,0d),1.0d>'::scircle;

SELECT  spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && '<(0d,2d),1.0d>'::scircle;

SELECT  spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && '<(2d,0d),1.0d>'::scircle;

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' && '<(0.5d,0.5d),0.1d>'::scircle;

--- should be false

SELECT  spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' && '<(1.5d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && '<(0d,88.0d),0.1d>'::scircle;

SELECT  spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' && '<(0.3d,0.5d),0.1d>'::scircle;

SELECT  spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' && '<(0d,-87d),0.1d>'::scircle;

--- spoly && scircle

--- should be true

SELECT  '<(0.5d,0.5d),0.1d>'::scircle && spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT  '<(0d,89.9d),0.1d>'::scircle && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT  '<(0d,90d),0.1d>'::scircle && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT  '<(0d,-89.9d),0.1d>'::scircle && spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT  '<(0d,-90d),0.1d>'::scircle && spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT  '<(0d,0d),2.0d>'::scircle && spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT  '<(0d,0d),1.0d>'::scircle && spoly '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}';

SELECT  '<(0d,90d),1.0d>'::scircle && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT  '<(180d,-90d),1.0d>'::scircle && spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

SELECT  '<(0d,0d),1.0d>'::scircle && spoly '{(0d,0d),(0d,1d),(1d,0d)}';

SELECT  '<(0d,2d),1.0d>'::scircle && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT  '<(2d,0d),1.0d>'::scircle && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT  '<(0.5d,0.5d),0.1d>'::scircle && spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

--- should be false

SELECT  '<(1.5d,0.5d),0.1d>'::scircle && spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT  '<(0d,88.0d),0.1d>'::scircle && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT  '<(0.3d,0.5d),0.1d>'::scircle && spoly '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT  '<(0d,-87d),0.1d>'::scircle && spoly '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}';

--- spoly @ spoly

--- should be true

SELECT spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' @ spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' @ spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' @ spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,0d)}' @ spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,0d)}' @ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

--- should be false

--SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' @ spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}' @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' @ spoly '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}';

SELECT spoly '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}' @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' @ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

--- spoly ~ spoly

--- should be true

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ~ spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ~ spoly '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ~ spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ~ spoly '{(0d,0d),(0d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ~ spoly '{(0d,0d),(0d,1d),(1d,0d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ spoly '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}';

--- should be false

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ spoly '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}';

SELECT spoly '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}' ~ spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ spoly '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ~ spoly '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}';

--- spoly && spoly

--- should be true

SELECT spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,0d)}' && spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,0d)}' && spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(0d,0d),(0d,1d),(1d,0d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' && spoly '{(0d,0d),(0d,1d),(1d,0d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && spoly '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}';

SELECT spoly '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' && spoly '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}';

SELECT spoly '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

--- should be false

SELECT spoly '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' && spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}';

SELECT spoly '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' && spoly '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(3d,-1d),(3d,1d),(5d,1d),(5d,-1d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(-1d,3d),(-1d,5d),(1d,5d),(1d,3d)}';

SELECT spoly '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' && spoly '{(179d,-1d),(179d,1d),(181d,1d),(181d,-1d)}';

--
-- ellipse and polygon
--

-- negators , commutator @,&&

SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'    @  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}'    @  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}'   @  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   &&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}'   &&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}'  &&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   !@  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}'   !@  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}'  !@  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'  !&&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}'  !&&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}' !&&  sellipse '<{10d,5d},(280d,-20d),90d>';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   ~  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   ~  spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   ~  spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   &&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   &&  spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'   &&  spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !~  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !~  spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !~  spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !&&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !&&  spoly '{(280d, -9d),(280d,-12d),(279d, -8d)}';
SELECT sellipse '<{10d,5d},(280d,-20d),90d>'  !&&  spoly '{(280d,-11d),(280d,-12d),(279d, -12d)}';

-- ellipse is point
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'    @  sellipse '<{0d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}'   @  sellipse '<{0d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   &&  sellipse '<{0d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}'  &&  sellipse '<{0d,0d},(280d,-20d),90d>';
SELECT sellipse '<{0d,0d},(280d,-20d),90d>'   @  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{0d,0d},(280d,-20d),90d>'   @  spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}';
SELECT sellipse '<{0d,0d},(280d,-20d),90d>'  &&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{0d,0d},(280d,-20d),90d>'  &&  spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}';

-- ellipse is circle
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'    @  sellipse '<{5d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}'   @  sellipse '<{2d,2d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   &&  sellipse '<{5d,5d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}'  &&  sellipse '<{5d,5d},(280d,-20d),90d>';
SELECT sellipse '<{5d,5d},(280d,-20d),90d>'   @  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{2d,2d},(280d,-20d),90d>'   @  spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}';
SELECT sellipse '<{5d,5d},(280d,-20d),90d>'  &&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{5d,5d},(280d,-20d),90d>'  &&  spoly '{(280d,-11d),(280d,-18d),(279d, -12d)}';

-- ellipse is line
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'    @  sellipse '<{5d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}'   @  sellipse '<{2d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   &&  sellipse '<{5d,0d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}'  &&  sellipse '<{5d,0d},(280d,-20d),90d>';
SELECT sellipse '<{5d,0d},(280d,-20d),90d>'   @  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{2d,0d},(280d,-20d),90d>'   @  spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}';
SELECT sellipse '<{5d,0d},(280d,-20d),90d>'  &&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{5d,0d},(280d,-20d),90d>'  &&  spoly '{(280d,-11d),(280d,-18d),(279d, -12d)}';

-- ellipse is a real ellipse
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'    @  sellipse '<{5d,2d},(280d,-20d),90d>';
SELECT spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}'   @  sellipse '<{2d,1d},(280d,-20d),90d>';
SELECT spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}'   &&  sellipse '<{5d,2d},(280d,-20d),90d>';
SELECT spoly '{(280d,-11d),(280d,-20d),(279d, -12d)}'  &&  sellipse '<{5d,2d},(280d,-20d),90d>';
SELECT sellipse '<{5d,2d},(280d,-20d),90d>'   @  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{2d,1d},(280d,-20d),90d>'   @  spoly '{(280d,-10d),(290d,-30d),(270d, -30d)}';
SELECT sellipse '<{5d,2d},(280d,-20d),90d>'  &&  spoly '{(280d, -9d),(280d, -8d),(279d, -8d)}' ;
SELECT sellipse '<{5d,2d},(280d,-20d),90d>'  &&  spoly '{(280d,-11d),(280d,-18d),(279d, -12d)}';

-- create polygon as aggregate
SELECT spoly(data.p) FROM ( SELECT spoint '(0,1)' as p UNION ALL SELECT spoint '(1,1)' UNION ALL SELECT '(1,0)' ) AS data ;

-- test stored data
SELECT count(id) FROM spheretmp5 WHERE id=2  AND area(p) BETWEEN 5.735555 AND 5.735556 ;

-- check to create this small polygon without errors
SELECT area( spoly '{
  (3.09472232280407 , 1.47261266025223),
  (3.0947320190777 , 1.47261266025223),
  (3.0947320190777 , 1.47262235652586),
  (3.09472232280407 , 1.47262235652586) }') >= 0 ;

SELECT npoints( spoly '{
  (1.51214841579108 , -2.90888208684947e-05),
  (1.5121581120647 , -2.90888208684947e-05),
  (1.5121581120647 , -1.93925472462553e-05),
  (1.51214841579108 , -1.93925472462553e-05)
}');

SELECT set_sphere_output( 'RAD' );

SELECT  spoint( spoly '{(0,0),(1,0),(1,1)}', 1 );
SELECT  spoint( spoly '{(0,0),(1,0),(1,1)}', 2 );
SELECT  spoint( spoly '{(0,0),(1,0),(1,1)}', 3 );
SELECT  spoly_as_array( spoly '{(0,0),(1,0),(1,1)}' );

-- spoly is convex
SELECT spoly_is_convex(spoly'{(53d 45m 35.0s, 37d 6m 30.0s), (52d 21m 36.0s, 41d 36m 7.0s), (54d 14m 18.0s, 45d 1m 35.0s), (51d 23m 3.0s, 45d 22m 49.0s), (51d 2m 12.0s, 41d 52m 1.0s), (50d 41m 47.0s, 38d 22m 0s) }');
SELECT spoly_is_convex(spoly'{(12d,32d),(34d,12d),(59d,21d),(69d,21d)}');
SELECT spoly_is_convex(spoly'{(12d,32d),(34d,12d),(59d,21d),(34d,40d)}');
SELECT spoly_is_convex(NULL);

-- Complex but valid polygon
SELECT '{
  (3.30474723646012 , 1.08600456205300),
  (3.30341855309927 , 1.08577960186707),
  (3.30341054542378 , 1.08578643990271),
  (3.30297351563319 , 1.08633534556428),
  (3.30357156120003 , 1.08643683957210),
  (3.30358891855857 , 1.08643995044436),
  (3.30360894676365 , 1.08644306147078),
  (3.30361829343581 , 1.08644430596871),
  (3.30362630482521 , 1.08644555030213),
  (3.30364633346451 , 1.08644866102000),
  (3.30365300940335 , 1.08645052692055),
  (3.30366102096957 , 1.08645177113937),
  (3.30367036769496 , 1.08645363721023),
  (3.30367837934959 , 1.08645488137174),
  (3.30368906174976 , 1.08645612569695),
  (3.30370107936906 , 1.08645799183673),
  (3.30370642025712 , 1.08645985750225),
  (3.30373179124734 , 1.08646358962156),
  (3.30374514456618 , 1.08646545561358),
  (3.30410706158729 , 1.08652886672786),
  (3.30427803417922 , 1.08655868846497),
  (3.30429673329093 , 1.08655930694968),
  (3.30432478121775 , 1.08655930174652),
  (3.30433278932944 , 1.08655308246640),
  (3.30446348355532 , 1.08638330933224)
}'::spoly;

CREATE FUNCTION run_tests(FLOAT8, FLOAT8, FLOAT8)
   RETURNS void
   AS 'pg_sphere', 'run_tests'
   LANGUAGE 'c'
   IMMUTABLE STRICT PARALLEL SAFE;

SELECT run_tests();
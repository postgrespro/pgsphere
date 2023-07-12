SELECT center(ARRAY[
    spoint(40.7128, -74.0060),
    spoint(34.0522, -118.2437),
    spoint(37.7749, -122.4194)
]);

SELECT center('{}'::SPoint[]);

CREATE FUNCTION spoint_from_xyz(FLOAT8, FLOAT8, FLOAT8)
   RETURNS spoint
   AS 'pg_sphere', 'spoint_from_xyz'
   LANGUAGE 'c'
   IMMUTABLE STRICT PARALLEL SAFE;

SELECT spoint_from_xyz(1, 0, 0);

SELECT spoint_from_xyz(0, 0, 0);

SELECT center(ARRAY[
    spoint_from_xyz(1, 0, 0),
    spoint_from_xyz(-1, 0, 0)
]);

SELECT center(NULL::SPoint[]);

SELECT @@ ARRAY[
    spoint(40.7128, -74.0060),
    spoint(34.0522, -118.2437),
    spoint(37.7749, -122.4194)
] AS center;

SELECT @@ ARRAY[]::spoint[] AS center;

SELECT @@ ARRAY[
    spoint_from_xyz(1, 0, 0),
    spoint_from_xyz(-1, 0, 0)
] AS center;

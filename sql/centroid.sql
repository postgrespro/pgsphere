SELECT centroid(ARRAY[
    spoint(40.7128, -74.0060),
    spoint(34.0522, -118.2437),
    spoint(37.7749, -122.4194)
]);

SELECT centroid('{}');

CREATE FUNCTION spoint_from_xyz(FLOAT8, FLOAT8, FLOAT8)
   RETURNS spoint
   AS 'MODULE_PATHNAME', 'spoint_from_xyz'
   LANGUAGE 'c'
   IMMUTABLE STRICT PARALLEL SAFE;

SELECT spoint_from_xyz(1, 0, 0);
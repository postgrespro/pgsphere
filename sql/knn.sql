CREATE TABLE points (id int, p spoint, pos int);
INSERT INTO points (id, p) SELECT x, spoint(random()*6.28, (2*random()-1)*1.57) FROM generate_series(1,314159) x;
CREATE INDEX i ON points USING gist (p spoint3);
SET enable_indexscan = true;
EXPLAIN (costs off) SELECT p <-> spoint (0.2, 0.3) FROM points ORDER BY 1 LIMIT 10;
UPDATE points SET pos = n FROM (SELECT id, row_number() OVER (ORDER BY p <-> spoint (0.2, 0.3)) n  FROM points ORDER BY p <-> spoint (0.2, 0.3) LIMIT 10) sel WHERE points.id = sel.id;
SET enable_indexscan = false;
SELECT pos, row_number() OVER (ORDER BY p <-> spoint (0.2, 0.3)) n  FROM points ORDER BY p <-> spoint (0.2, 0.3) LIMIT 10;
DROP TABLE points;


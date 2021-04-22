-- test spoint3 operator class with and without index-only scan

SET enable_seqscan = OFF;
SET enable_bitmapscan = OFF;
SET enable_indexonlyscan = ON;

EXPLAIN (COSTS OFF) SELECT count(*) FROM spheretmp1b WHERE p <@ scircle '<(1,1),0.3>';
                    SELECT count(*) FROM spheretmp1b WHERE p <@ scircle '<(1,1),0.3>';
EXPLAIN (COSTS OFF) SELECT count(*) FROM spheretmp1b WHERE p = spoint '(3.09 , 1.25)';
                    SELECT count(*) FROM spheretmp1b WHERE p = spoint '(3.09 , 1.25)';

SET enable_bitmapscan = ON;
SET enable_indexonlyscan = OFF;

EXPLAIN (COSTS OFF) SELECT count(*) FROM spheretmp1b WHERE p <@ scircle '<(1,1),0.3>';
                    SELECT count(*) FROM spheretmp1b WHERE p <@ scircle '<(1,1),0.3>';
EXPLAIN (COSTS OFF) SELECT count(*) FROM spheretmp1b WHERE p = spoint '(3.09 , 1.25)';
                    SELECT count(*) FROM spheretmp1b WHERE p = spoint '(3.09 , 1.25)';

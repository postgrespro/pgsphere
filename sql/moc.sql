SET client_min_messages = 'notice';
-- SET client_min_messages = 'warning';

SELECT smoc('');            -- expected: '0/'
SELECT '0/'::smoc;          -- expected: '0/'
SELECT '29/'::smoc;         -- expected: '0/'
SELECT '0/0-3,7'::smoc;     -- expected: '0/0-3,7'
SELECT '0/0,1,2,3,7'::smoc; -- expected: '0/0-3,7'
SELECT '(0.78, 0.81)'::spoint <@ '7/123-456,10000-20000'::smoc;
SELECT '(0.78, 0.81)'::spoint <@ '7/123-456,1000-2000'::smoc;

SELECT 1 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339,0-1'));
SELECT 1 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));
SELECT 2 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));
SELECT 6 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));
SELECT 29 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));
SELECT 333 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));
SELECT 555 <@ (smoc('29/2-5,20-29,123,444,17-21,33-39,332-339'));

SELECT set_smoc_output_type(1);

SELECT '29/0-3,7'::smoc;
SELECT '29/0,1,2,3,7'::smoc;

SELECT '29/3-11,70-88,22-34'::smoc;

SELECT '29/5-11,70-88,2-4'::smoc;

SELECT '29/11-18,22-27,31-35,42-55,62-69,100-111,15-49'::smoc;

SELECT '29/1-3,20-30,7-17'::smoc;

SELECT '29/16-32,10-20'::smoc;

SELECT '29/1-3,11-14,17-21,40-50,9-33'::smoc;

SELECT '29/10-20,16-32'::smoc;

SELECT '29/20-30,64-72,89-93,26-100'::smoc;

SELECT '29/3-11,20-30,64-72,89-93,26-100'::smoc;

SELECT '29/3-11,20-30,64-72,89-93,222-333,26-100'::smoc;

SELECT '29/20-30,64-72,89-93,222-333,26-100'::smoc;

SELECT smoc(-1); -- expected: error
SELECT smoc(15); -- expected: error
SELECT smoc(0);  -- expected: '0/'

SELECT smoc('');
SELECT smoc('abc');         -- expected: error
SELECT smoc('-1/');         -- expected: error
SELECT smoc('30/');         -- expected: error
SELECT smoc('0/');
SELECT smoc('0/0-3,7');
SELECT smoc('0/0,1,2,3,7');

select set_smoc_output_type(0);

SELECT ''::smoc;
SELECT 'abc'::smoc;         -- expected: error
SELECT '-1/'::smoc;         -- expected: error
SELECT '30/'::smoc;         -- expected: error
SELECT '0/'::smoc;          -- expected: '0/'
SELECT '0/0-3,7'::smoc;     -- expected: '0/0-3,7'
SELECT '0/0,1,2,3,7'::smoc; -- expected: '0/0-3,7'

select set_smoc_output_type(1);

SELECT smoc('2/0,1,2,3,7 4/17,21-33,111');

SELECT smoc('2/0,1,2,3,7 0/17,21-33,111');

SELECT max_order(smoc(''));
SELECT max_order(smoc('1/'));
SELECT max_order(smoc('1/1'));
SELECT max_order(smoc('1/10-3'));

SELECT max_order(smoc('1/0-3'));
SELECT max_order(smoc('1/0-1'));
SELECT max_order(smoc('29/0-1'));
SELECT max_order(smoc('29/0-3'));
SELECT max_order(smoc('29/0-7'));
SELECT max_order(smoc('29/0-15'));

select set_smoc_output_type(0);

SELECT smoc('29/32-63');
SELECT smoc('29/64-127');
SELECT smoc('0/0-11');
SELECT smoc('0/1-3');
SELECT smoc('0/1');
SELECT smoc('0/3-5');
SELECT smoc('0/3-11');
SELECT smoc('0/0,3-11');
SELECT smoc('1/0,3-42');
SELECT smoc('29/3-42');
SELECT smoc('29/1');
SELECT smoc('28/1');
SELECT smoc('24/1');
SELECT smoc('24/1 29/1');
SELECT smoc('24/1 11/1 29/1');
SELECT smoc('24/1 11/1 29/1,3');
SELECT smoc('24/1 11/1 29/1,3 2/22-33');
SELECT smoc('2/22-33');
SELECT smoc('24/1 11/1 29/1,3 2/22-33');
SELECT smoc('');
SELECT smoc('1/6-7 2/22-23,32-33 11/1 24/1 29/1,3');
SELECT smoc('5/1-127,999-1103');
SELECT smoc('5/1024-1103');
SELECT smoc('28/1101-1103');

SELECT smoc_union('1/1,4-6', '1/3-5 2/8');
SELECT '1/1'::smoc || '1/2' AS union;

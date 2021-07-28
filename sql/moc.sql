SET client_min_messages = 'notice';
SET extra_float_digits = 0; -- make results compatible with 9.6 .. 11

SELECT smoc('');            -- expected: '0/'
SELECT '0/'::smoc;          -- expected: '0/'
SELECT '29/'::smoc;         -- expected: '29/'
SELECT '0/0-3,7'::smoc;     -- expected: '0/0-3,7'
SELECT '0/0,1,2,3,7'::smoc; -- expected: '0/0-3,7'

SELECT smoc_info('0/'::smoc);
SELECT smoc_info('0/1-2'::smoc);

SELECT area('0/'::smoc);
SET extra_float_digits = -1; -- last digit deviating on i386
SELECT area('29/1'::smoc);
SET extra_float_digits = 0;
SELECT area('0/1-3'::smoc);
SELECT area('0/0-11'::smoc);

SELECT '(0.78, 0.81)'::spoint <@ '7/123-456,10000-20000'::smoc;
SELECT '(0.78, 0.81)'::spoint <@ '7/123-456,1000-2000'::smoc;

SELECT '0/'::smoc = '1/'::smoc AS eq;
SELECT '0/1,3,7'::smoc = '0/1,3,7' AS eq;
SELECT '0/1,4,7'::smoc = '0/1,3,7' AS eq;
SELECT '0/1,2,3'::smoc <> '0/1-3'::smoc AS neq;

SELECT '0/4-6'::smoc && '0/3,7-8'::smoc AS overlap;
SELECT '0/4-6'::smoc && '0/3,6-8'::smoc AS overlap;
SELECT '0/4-6'::smoc !&& '0/6'::smoc AS not_overlap;

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
SELECT '1/1'::smoc | '1/2' AS union;
SELECT sum(moc) FROM (VALUES ('0/1'::smoc), ('0/2'), ('0/4')) sub(moc);

SELECT smoc_intersection('1/1,4-6', '1/3-5 2/8');
SELECT '0/1'::smoc & '1/3,5,7,9' AS intersection;
SELECT '1/9,11,13,15'::smoc & '0/1,2' AS intersection;
SELECT intersection(moc) FROM (VALUES ('0/1-4'::smoc), ('0/2-5'), (NULL)) sub(moc);

SELECT smoc_degrade(6, '7/1,3,5,9');
SELECT smoc_degrade(5, '7/1,3,5,9');
SELECT smoc_degrade(0, '1/0');
SELECT smoc_degrade(0, '1/47');

WITH mocs(x) AS (VALUES ('0/'::smoc), ('0/1'), ('0/2'), ('0/4'), ('0/1,3'), ('0/1-3'), ('0/2-4'))
  SELECT a.x AS a, b.x AS b,
         a.x = b.x AS "=", a.x <> b.x AS "<>",
         a.x && b.x AS "&&",
         a.x <@ b.x AS "<@", a.x !<@ b.x AS "!<@",
         a.x !@> b.x AS "!@>", a.x @> b.x AS "@>",
         a.x | b.x AS "|", a.x & b.x AS "&"
    FROM mocs a, mocs b;

SELECT smoc_disc(0, 0, 0, 1);
SELECT smoc_disc(1, 0, 0, 1);
SELECT smoc_disc(2, 0, 0, 1);
SELECT smoc_disc(0, 0, 0, 3.2);
SELECT smoc_disc(2, 0, 0, 3.2);

SELECT smoc(6, '(0,0)'::spoint);

SELECT smoc(1, '<(1,1),1>'::scircle);
SELECT smoc(3, '<(0,1.3),.5>'::scircle);

SELECT smoc(5, '{(.1,.1), (.2,.1), (.2,.2), (.1, .2)}'::spoly);
SELECT smoc(3, '{(.1,.1), (-1,.1), (-1,-1), (.1, -1)}'::spoly);
SELECT smoc(3, '{(.1,.1), (.1,-1), (-1,-1), (-1, .1)}'::spoly);

CREATE TABLE g (p spoly);
INSERT INTO g (p) VALUES (spoly '{(1.48062434277764d , -0.112757492761271d),(1.48062583213677d , -0.0763898467955446d),(1.44427278313068d, -0.0762453395638312d),(1.44413625055362d , -0.112726631135703d)}');
SELECT smoc(6, p) FROM g;

-- lswscans/data/part1/Bruceplatten/FITS/B3558b.fits
SELECT smoc(6, '{(1.28867804735846 , 0.421769766439468),(1.40564002826964 , 0.201070262502835),(1.27597340819331 , 0.210561432516079),(1.28707379974056 , 0.351693515652388)}'::spoly);

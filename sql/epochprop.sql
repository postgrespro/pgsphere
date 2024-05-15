SET extra_float_digits = 1;

SELECT
       to_char(DEGREES(tp[1]), '999D9999999999'),
       to_char(DEGREES(tp[2]), '999D9999999999'),
       to_char(tp[3], '999D999'),
       to_char(DEGREES(tp[4])*3.6e6, '999D999'),
       to_char(DEGREES(tp[5])*3.6e6, '99999D999'),
       to_char(tp[6], '999D999')
FROM (
       SELECT epoch_prop(spoint(radians(269.45207695), radians(4.693364966)),
               546.9759,
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6), -110,
               -100) AS tp) AS q;

SELECT
       to_char(DEGREES(tp[1]), '999D9999999999'),
       to_char(DEGREES(tp[2]), '999D9999999999'),
       to_char(tp[3], '999D999'),
       to_char(DEGREES(tp[4])*3.6e6, '999D999'),
       to_char(DEGREES(tp[5])*3.6e6, '99999D999'),
       to_char(tp[6], '999D999')
FROM (
       SELECT epoch_prop(spoint(radians(269.45207695), radians(4.693364966)),
               0,
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6), -110,
               -100) AS tp) AS q;

SELECT
       to_char(DEGREES(tp[1]), '999D9999999999'),
       to_char(DEGREES(tp[2]), '999D9999999999'),
       to_char(tp[3], '999D999'),
       to_char(DEGREES(tp[4])*3.6e6, '999D999'),
       to_char(DEGREES(tp[5])*3.6e6, '99999D999'),
       to_char(tp[6], '999D999')
FROM (
       SELECT epoch_prop(spoint(radians(269.45207695), radians(4.693364966)),
               NULL,
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6), -110,
               -100) AS tp) AS q;

SELECT
       to_char(DEGREES(tp[1]), '999D9999999999'),
       to_char(DEGREES(tp[2]), '999D9999999999'),
       to_char(tp[3], '999D999'),
       to_char(DEGREES(tp[4])*3.6e6, '999D999'),
       to_char(DEGREES(tp[5])*3.6e6, '99999D999'),
       to_char(tp[6], '999D999')
FROM (
       SELECT epoch_prop(spoint(radians(269.45207695), radians(4.693364966)),
               23,
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6), NULL,
               20) AS tp) AS q;

SELECT
       to_char(DEGREES(tp[1]), '999D9999999999'),
       to_char(DEGREES(tp[2]), '999D9999999999'),
       to_char(tp[3], '999D999'),
       to_char(DEGREES(tp[4])*3.6e6, '999D999'),
       to_char(DEGREES(tp[5])*3.6e6, '99999D999'),
       to_char(tp[6], '999D999')
FROM (
       SELECT epoch_prop(spoint(radians(269.45207695), radians(4.693364966)),
               23,
               NULL, RADIANS(10362/3.6e6), -110,
               120) AS tp) AS q;

SELECT epoch_prop(NULL,
               23,
               0.01 , RADIANS(10362/3.6e6), -110,
               120);

SELECT epoch_prop_pos(spoint(radians(269.45207695), radians(4.693364966)),
               23,
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6), -110,
               20) AS tp;

SELECT epoch_prop_pos(spoint(radians(269.45207695), radians(4.693364966)),
               RADIANS(-801.551/3.6e6), RADIANS(10362/3.6e6),
               20) AS tp;

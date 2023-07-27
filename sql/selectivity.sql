-- test selectivity estimator functions

create table spoint10k (star spoint);
insert into spoint10k select spoint(i, i*i) from generate_series(1, 10000) g(i);
create index on spoint10k using gist (star);
analyze spoint10k;

-- "explain analyze" wrapper that removes 'cost=...' since it varies across architectures
-- (we can't use "costs off" since that also removes the estimated row count)
create or replace function explain(query text, do_analyze text default 'true') returns setof text language plpgsql as $$
declare
  line text;
begin
  for line in execute format('explain (analyze %s, timing off, summary off) %s', do_analyze, query) loop
    return next regexp_replace(line, 'cost=\S+ ', '');
  end loop;
  return;
end;
$$;

-- <@ operator selectivity
select explain('select * from spoint10k where star <@ scircle(spoint(1,1), 1)');
select explain('select * from spoint10k where star <@ scircle(spoint(1,1), .1)');
select explain('select * from spoint10k where star <@ scircle(spoint(1,1), .01)');

select explain('select * from spoint10k where scircle(spoint(1,1), 1) @> star');
select explain('select * from spoint10k where scircle(spoint(1,1), .1) @> star');
select explain('select * from spoint10k where scircle(spoint(1,1), .01) @> star');

select explain('select * from spoint10k where star !<@ scircle(spoint(1,1), 1)');
select explain('select * from spoint10k where star !<@ scircle(spoint(1,1), .1)');
select explain('select * from spoint10k where star !<@ scircle(spoint(1,1), .01)');

select explain('select * from spoint10k where scircle(spoint(1,1), 1) !@> star');
select explain('select * from spoint10k where scircle(spoint(1,1), .1) !@> star');
select explain('select * from spoint10k where scircle(spoint(1,1), .01) !@> star');

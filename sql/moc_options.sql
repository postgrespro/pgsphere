create table moc_opt (m smoc);
insert into moc_opt select format('9/%s', i)::smoc from generate_series(1, 1000) g(i);
analyze moc_opt;

create index moc_opt5 on moc_opt using gin (m);
explain (analyze, costs off, timing off, summary off) select * from moc_opt where m && '9/1';
drop index moc_opt5;

create index moc_opt8 on moc_opt using gin (m smoc_gin_ops_fine);
explain (analyze, costs off, timing off, summary off) select * from moc_opt where m && '9/1';
drop index moc_opt8;

create index moc_opt9 on moc_opt using gin (m smoc_gin_ops (order = 9));
explain (analyze, costs off, timing off, summary off) select * from moc_opt where m && '9/1';

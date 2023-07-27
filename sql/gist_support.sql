-- spoint_dwithin function selectivity
set jit = off; -- suppress extra planning output

select explain('select * from spoint10k where spoint_dwithin(star, spoint(1,1), 1)');
select explain('select * from spoint10k where spoint_dwithin(star, spoint(1,1), .1)');
select explain('select * from spoint10k where spoint_dwithin(star, spoint(1,1), .01)');

select explain('select * from spoint10k where spoint_dwithin(spoint(1,1), star, 1)');
select explain('select * from spoint10k where spoint_dwithin(spoint(1,1), star, .1)');
select explain('select * from spoint10k where spoint_dwithin(spoint(1,1), star, .01)');

select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, 1)', do_analyze := 'false');
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, .1)');
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, .01)');

-- spoint_dwithin is symmetric in the first two arguments
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, .01)
  where spoint_dwithin(a.star, spoint(1,1), .1)');
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(b.star, a.star, .01)
  where spoint_dwithin(a.star, spoint(1,1), .1)');

-- both sides indexable, check if the planner figures out the better choice
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, .01)
  where spoint_dwithin(a.star, spoint(1,1), .1) and spoint_dwithin(b.star, spoint(1,1), .05)');
select explain('select * from spoint10k a join spoint10k b on spoint_dwithin(a.star, b.star, .01)
  where spoint_dwithin(a.star, spoint(1,1), .05) and spoint_dwithin(b.star, spoint(1,1), .1)');

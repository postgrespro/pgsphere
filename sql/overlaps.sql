/*
This set of tests is designed to verify the compliance of the overlap operation with the DE-9IM model
*/


-- sline vs sline


-- the lines have no common points
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(20d,0d)', spoint'(30d,0d)') as actual;

-- the point of intersection of the lines is the boundary for both
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(10d,0d)', spoint'(20d,0d)') as actual;

-- one line intersects the other at the boundary point of the latter
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(0d,90d)', spoint'(0d,-10d)') as actual;

-- the interiors of the lines intersect at one point
select 'sline && sline', 'f' as expected, sline( spoint'(-10d,0d)', spoint'(10d,0d)') && sline( spoint'(0d,90d)', spoint'(0d,-10d)') as actual;

-- the lines match
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(0d,0d)', spoint'(10d,0d)') as actual;

-- one line lies completely in the other and they have an intersection at the boundary point
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(0d,0d)', spoint'(5d,0d)') as actual;

-- one line lies completely in the other and they have no intersection at the boundary point
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(2d,0d)', spoint'(5d,0d)') as actual;

-- one line partially lies in the other
select 'sline && sline', 't' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(5d,0d)', spoint'(20d,0d)') as actual;

-- the line degenerated into the point lies on the boundary of another
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(0d,0d)', spoint'(0d,0d)') as actual;

-- the line degenerated into the point lies in the interior of another
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(10d,0d)') && sline( spoint'(5d,0d)', spoint'(5d,0d)') as actual;

-- both lines are degenerated into the point and coincide
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(0d,0d)') && sline( spoint'(0d,0d)', spoint'(0d,0d)') as actual;

-- both lines are degenerated into the point and do not coincide, but lie close to each other
select 'sline && sline', 'f' as expected, sline( spoint'(0d,0d)', spoint'(0d,0d)') && sline( spoint'(1d,0d)', spoint'(1d,0d)') as actual;


-- scircle vs scircle


-- the circles have no common points
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(30d , 0d) , 5d>' as actual;

-- the point of intersection of the circles is the boundary for both
select 'scircle && scircle', 'f' as expected, scircle'<(-10d , 0d) , 10d>' && scircle'<(10d , 0d) , 10d>' as actual;

-- the circles match
select 'scircle && scircle', 'f' as expected, scircle'<(-10d , 0d) , 10d>' && scircle'<(-10d , 0d) , 10d>' as actual;

-- one circle lies completely in the other and they have an intersection at the boundary point
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(-10d , 0d) , 10d>' as actual;

-- one circle lies completely in the other and they have no intersection at the boundary point
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(0d , 0d) , 5d>' as actual;

-- one circle partially lies in the other
select 'scircle && scircle', 't' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(10d , 0d) , 20d>' as actual;

-- the circle degenerated into the point lies on the boundary of another
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(20d , 0d) , 0d>' as actual;

-- the circle degenerated into the point lies in the interior of another
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 20d>' && scircle'<(0d , 0d) , 0d>' as actual;

-- both circles are degenerated into the point and coincide
select 'scircle && scircle', 'f' as expected, scircle'<(0d , 0d) , 0d>' && scircle'<(0d , 0d) , 0d>' as actual;

-- both circles are degenerated into the point and do not coincide, but lie close to each other
select 'scircle && scircle', 'f' as expected, scircle'<(1d , 0d) , 0d>' && scircle'<(0d , 0d) , 0d>' as actual;


-- sellipse vs sellipse


-- the ellipses have no common points
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 45d , 20d }, (-45d , 0d) , 0d>' && sellipse'<{ 45d , 20d }, (50 , 0d) , 0d>' as actual;

-- the point of intersection of the ellipses is the boundary for both
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 45d , 20d }, (-45d , 0d) , 0d>' && sellipse'<{ 45d , 20d }, (45d , 0d) , 0d>' as actual;

-- the ellipses match
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 45d , 20d }, (-45d , 0d) , 0d>' && sellipse'<{ 45d , 20d }, (-45d , 0d) , 0d>' as actual;

-- one ellipse lies completely in the other and they have an intersection at the boundary point
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 20d , 5d }, (-20d , 0d) , 0d>' as actual;

-- one ellipse lies completely in the other and they have no intersection at the boundary point
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 10d , 5d }, (-20d , 0d) , 0d>' as actual;

-- one ellipse partially lies in the other
select 'sellipse && sellipse', 't' as expected, sellipse'<{ 45d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 20d , 5d }, (30d , 0d) , 0d>' as actual;

-- the ellipse degenerated into the line lies completely in the interior of another (non-degenerate) ellipse
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 10d , 0d }, (10d , 0d) , 0d>' as actual;

-- the ellipse degenerated into the line partially lies in the interior of another (non-degenerate) ellipse
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 20d , 0d }, (30d , 0d) , 0d>' as actual;

-- the ellipse degenerated into the point lies on the boundary of another
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 0d , 0d }, (30d , 0d) , 0d>' as actual;

-- the ellipse degenerated into the point lies in the interior of another
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 40d , 30d }, (0d , 0d) , 0d>' && sellipse'<{ 0d , 0d }, (40d , 0d) , 0d>' as actual;

-- both ellipses are degenerated into the point and coincide
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 0d , 0d }, (0d , 0d) , 0d>' && sellipse'<{ 0d , 0d }, (0d , 0d) , 0d>' as actual;

-- both ellipses are degenerated into the point and do not coincide, but lie close to each other
select 'sellipse && sellipse', 'f' as expected, sellipse'<{ 0d , 0d }, (0d , 0d) , 0d>' && sellipse'<{ 0d , 0d }, (1d , 0d) , 0d>' as actual;


-- sellipse vs scircle


-- the ellipse and circle have no common points
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 10d }, (0d , 0d) , 0d>' && scircle'<(30d , 0d) , 5d>' as actual;

-- the point of intersection of the ellipse and circle is the boundary for both
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 10d }, (0d , 0d) , 0d>' && scircle'<(30d , 0d) , 10d>' as actual;

-- the ellipse and circle match
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 10d , 10d }, (30d , 0d) , 0d>' && scircle'<(30d , 0d) , 10d>' as actual;

-- the ellipse lies completely in the circle and they have an intersection at the boundary point
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 10d }, (0d , 0d) , 0d>' && scircle'<(20d , 0d) , 40d>' as actual;

-- the ellipse lies completely in the circle and they have no intersection at the boundary point
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 10d }, (20d , 0d) , 0d>' && scircle'<(20d , 0d) , 40d>' as actual;

-- the circle lies completely in the ellipse
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 10d }, (0d , 0d) , 0d>' && scircle'<(0d , 0d) , 5d>' as actual;

-- the circle partially lies in the ellipse
select 'sellipse && scircle', 't' as expected, sellipse'<{ 20d , 10d }, (0d , 0d) , 0d>' && scircle'<(20d , 0d) , 15d>' as actual;

-- the ellipse degenerated into the line lies completely in the interior of circle
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 0d }, (0d , 0d) , 0d>' && scircle'<(0d , 0d) , 20d>' as actual;

-- the ellipse degenerated into the line partially lies in the interior of circle
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 20d , 0d }, (10d , 0d) , 0d>' && scircle'<(0d , 0d) , 20d>' as actual;

-- the ellipse degenerated into the point lies on the boundary of circle
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 0d , 0d }, (10d , 0d) , 0d>' && scircle'<(0d , 0d) , 10d>' as actual;

-- the ellipse degenerated into the point lies in the interior of circle
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 0d , 0d }, (5d , 0d) , 0d>' && scircle'<(0d , 0d) , 10d>' as actual;

-- the ellipse and circle are degenerated into the point and coincide
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 0d , 0d }, (0d , 0d) , 0d>' && scircle'<(0d , 0d) , 0d>' as actual;

-- the ellipses are degenerated into the point and do not coincide, but lie close to each other
select 'sellipse && scircle', 'f' as expected, sellipse'<{ 0d , 0d }, (0d , 0d) , 0d>' && scircle'<(1d , 0d) , 0d>' as actual;


-- spath vs spath


-- the opened paths have no common points
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d,0d),(45d,15d) }' && spath'{ (60d, 15d),(70d, 20d),(75d, 20d) }' as actual;

-- the points of intersection of the opened paths is the boundary for both
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (-10d, 0d),(0d, 10d),(45d, 15d) }' as actual;

-- one opened path intersects the other at the boundary point of the latter
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (-20d, -10d),(-10d, 0d),(0d, 10d),(45d, 15d),(60d, 15d) }' as actual;

-- the interiors of the opened paths intersect at one point
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (0d, 10d),(0d, -10d),(10d, -10d) }' as actual;

-- the interiors of the opened paths intersect at two points
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (0d, 10d),(0d, -10d),(5d, 0d) }' as actual;

-- the opened paths match
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' as actual;

-- the closed paths match
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(-10d, 0d) }' && spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(-10d, 0d) }' as actual;

-- one opened path lies completely in the other and they have an intersection at the boundary point
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(60d, 15d),(70d, 20d) }' && spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' as actual;

-- one opened path lies completely in the other and they have no intersection at the boundary point
select 'spath && spath', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(60d, 15d),(70d, 20d) }' && spath'{ (10d, 0d),(45d, 15d),(60d, 15d) }' as actual;

-- one opened path partially lies in the other
select 'spath && spath', 't' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && spath'{ (45d, 15d),(60d, 15d),(70d, 20d) }' as actual;

-- one closed path partially lies in the other
select 'spath && spath', 't' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(-10d, 0d) }' && spath'{ (45d, 15d),(60d, 15d),(70d, 20d),(45d, 15d) }' as actual;

-- the opened path partially lies in the closed path
select 'spath && spath', 't' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(-10d, 0d) }' && spath'{ (45d, 15d),(60d, 15d),(70d, 20d) }' as actual;


-- spath vs sline


-- the opened path and line have no common points
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(20d, 0d)', spoint'(30d, 0d)') as actual;

-- the points of intersection of the opened path and line is the boundary for both
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(-10d, 0d)', spoint'(45d, 15d)') as actual;

-- the interiors of the opened path and line intersect at one point
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(0d, 10d)', spoint'(0d, -10d)') as actual;

-- the path and line match
select 'spath && sline', 'f' as expected, spath'{ (0d, 10d),(0d, -10d) }' && sline( spoint'(0d, 10d)', spoint'(0d, -10d)') as actual;

-- the line lies completely in the opened path and they have an intersection at the boundary point
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(60d, 20d) }' && sline( spoint'(-10d, 0d)', spoint'(10d, 0d)') as actual;

-- the line lies completely in the opened path and they have no intersection at the boundary point
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d),(60d, 20d) }' && sline( spoint'(10d, 0d)', spoint'(45d, 15d)') as actual;

-- the line partially lies in the open path
select 'spath && sline', 't' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(5d, 0d)', spoint'(20d, 0d)') as actual;

-- the line degenerated into the point lies in the boundary of open path
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(-10d, 0d)', spoint'(-10d, 0d)') as actual;

-- the line degenerated into the point lies in the interior of open path
select 'spath && sline', 'f' as expected, spath'{ (-10d, 0d),(10d, 0d),(45d, 15d) }' && sline( spoint'(0d, 0d)', spoint'(0d, 0d)') as actual;


-- spoly vs spoly


-- the polygons have no common points
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' && spoly'{ (10d, 0d),(20d, 0d),(10d, 10d) }' as actual;

-- the point of intersection of the polygons is the boundary for both
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' && spoly'{ (10d, 0d),(20d, 0d),(0d, 10d) }' as actual;

-- the interiors of the polygons intersect at one line
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' && spoly'{ (0d, 0d),(20d, 0d),(0d, 10d) }' as actual;

-- the polygons matches
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' && spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' as actual;

-- one polygon lies completely in the other and they have an intersection at the boundary line
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(20d, 0d),(20d, 20d),(0d, 20d) }' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- one polygon lies completely in the other and they have no intersection at the boundary line or point
select 'spoly && spoly', 'f' as expected, spoly'{ (0d, 0d),(20d, 0d),(20d, 20d),(0d, 20d) }' && spoly'{ (5d, 5d),(15d, 5d),(15d, 15d),(5d,15d) }' as actual;

-- one polygon partially lies in the other
select 'spoly && spoly', 't' as expected, spoly'{ (0d, 0d),(-10d, 0d),(0d, 10d) }' && spoly'{ (5d, 5d),(30d, 5d),(30d, 15d),(5d,15d) }' as actual;


-- spoly vs scircle


-- the polygon and circle have no common points
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(0d , 90d) , 10d>' as actual;

-- the point of intersection of the polygon and circle is the boundary for both
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(0d , 90d) , 80d>' as actual;

-- the circle lies completely in the polygon and they have an intersection at the boundary point
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(5d , 1d) , 1d>' as actual;

-- the circle lies completely in the polygon and they have no intersection at the boundary point
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(5d , 2d) , 1d>' as actual;

-- the polygon lies completely in the circle
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(0d , 15d) , 20d>' as actual;

-- the polygon partially lies in the circle
select 'spoly && scircle', 't' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(0d , 5d) , 5d>' as actual;

-- the circle degenerated into the point lies in the boundary of polygon
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(0d , 0d) , 0d>' as actual;

-- the circle degenerated into the point lies in the interior of polygon
select 'spoly && scircle', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && scircle'<(2d , 2d) , 0d>' as actual;


-- spoly vs sellipse


-- the polygon and ellipse have no common points
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 30d , 20d }, (0d , 90d) , 0d>' as actual;

-- the point of intersection of the polygon and ellipse is the boundary for both
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 80d , 20d }, (0d , 90d) , 90d>' as actual;

-- the ellipse lies completely in the polygon and they have an intersection at the boundary points
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(20d, 0d),(20d, 20d),(0d, 20d) }' && sellipse'<{ 10d , 5d }, (10d , 10d), 0d>' as actual;

-- the ellipse lies completely in the polygon and they have no intersection at the boundary points
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(20d, 0d),(20d, 20d),(0d, 20d) }' && sellipse'<{ 7d , 5d }, (10d , 10d), 0d>' as actual;

-- the polygon lies completely in the ellipse and they have an intersection at the boundary points
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 5d),(10d, 10d),(20d, 5d),(10d, 0d) }' && sellipse'<{ 10d , 5d }, (10d , 5d), 0d>' as actual;

-- the polygon lies completely in the ellipse and they have no intersection at the boundary points
select 'spoly && sellipse', 'f' as expected, spoly'{ (1d, 5d),(10d, 9d),(19d, 5d),(10d, 1d) }' && sellipse'<{ 10d , 5d }, (10d , 10d), 0d>' as actual;

-- the ellipse partially lies in the polygon
select 'spoly && sellipse', 't' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 10d , 2d }, (0d , 5d) , 0d>' as actual;

-- the ellipse degenerated into the point lies in the boundary of polygon
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 0d , 0d }, (0d , 0d), 0d>' as actual;

-- the ellipse degenerated into the point lies in the interior of polygon
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 0d , 0d }, (5d , 2d), 0d>' as actual;

-- the ellipse degenerated into the line lies in the boundary of polygon
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 5d , 0d }, (5d , 0d), 0d>' as actual;

-- the ellipse degenerated into the line lies in the interior of polygon
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 2d , 0d }, (1d , 4d), 90d>' as actual;

-- the ellipse degenerated into the line partially lies in the interior of polygon
select 'spoly && sellipse', 'f' as expected, spoly'{ (0d, 0d),(10d, 0d),(0d, 10d) }' && sellipse'<{ 10d , 0d }, (1d , 4d), 0d>' as actual;


-- sbox vs sbox


-- the boxes have no common points
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((30d , 30d), (50d , 50d))' as actual;

-- the point of intersection of the boxes is the boundary for both
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((20d , 20d), (50d , 50d))' as actual;

-- the boundaries of the boxes intersect at one line
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((20d , 0d), (50d , 20d))' as actual;

-- the boxes matches
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((5d , 5d), (10d , 10d))' as actual;

-- one box lies completely in the other and they have no intersection at the boundary line or point
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((5d , 5d), (10d , 10d))' as actual;

-- one box partially lies in the other
select 'sbox && sbox', 't' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((10d , 10d), (30d , 30d))' as actual;

-- one box degenerated into the line lies in the boundary of other
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((0d , 0d), (20d , 0d))' as actual;

-- one box degenerated into the line lies in the interior of other
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((5d , 5d), (10d , 5d))' as actual;

-- one box degenerated into the point lies in the boundary of other
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((0d , 0d), (0d , 0d))' as actual;

-- one box degenerated into the point lies in the interior of other
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sbox'((5d , 5d), (5d , 5d))' as actual;

-- the boxes are degenerated into the point and coincide
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && sbox'((0d , 0d), (0d , 0d))' as actual;

-- the boxes are degenerated into the point and not coincide
select 'sbox && sbox', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && sbox'((10d , 10d), (10d , 10d))' as actual;


-- sbox vs scircle


-- the box and circle have no common points
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(0d , 90d) , 10d>' as actual;

-- the point of intersection of the box and circle is the boundary for both
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(30d , 0d) , 10d>' as actual;

-- the circle lies completely in the box and they have an intersection at the boundary points
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(10d , 10d) , 10d>' as actual;

-- the circle lies completely in the box and they have no intersection at the boundary points
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(10d , 10d) , 5d>' as actual;

-- the box lies completely in the circle and they have an intersection at the boundary points
select 'sbox && scircle', 'f' as expected, sbox'((-4d , -3d), (3d , 4d))' && scircle'<(0d , 0d) , 5d>' as actual;

-- the box lies completely in the circle and they have no intersection at the boundary points
select 'sbox && scircle', 'f' as expected, sbox'((-4d , -3d), (3d , 4d))' && scircle'<(0d , 0d) , 10d>' as actual;

-- the box degenerated into the line intersects circle
select 'sbox && scircle', 'f' as expected, sbox'((-20d , 0d), (20d , 0d))' && scircle'<(0d , 0d) , 10d>' as actual;

-- the circle partially lies in the box
select 'sbox && scircle', 't' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(15d , 15d) , 10d>' as actual;

-- the circle degenerated into the point lies in the boundary of box
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(0d , 10d) , 0d>' as actual;

-- the circle degenerated into the point lies in the interior of box
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && scircle'<(10d , 10d) , 0d>' as actual;

-- the box degenerated into the point lies in the boundary of circle
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && scircle'<(10d , 0d) , 10d>' as actual;

-- the box degenerated into the point lies in the interior of circle
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && scircle'<(0d , 0d) , 10d>' as actual;

-- the box and circle are degenerated into the point and coincide
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && scircle'<(0d , 0d) , 0d>' as actual;

-- the box and circle are degenerated into the point and not coincide
select 'sbox && scircle', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && scircle'<(5d , 5d) , 0d>' as actual;


-- sbox vs spoly


-- the box and polygon have no common points
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (30d, 0d),(40d, 0d),(30d, 20d) }' as actual;

-- the point of intersection of the box and polygon is the boundary for both
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (30d, 0d),(40d, 0d),(30d, 20d) }' as actual;

-- the boundaries of the boxes intersect at one line
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (20d, 0d),(30d, 0d),(20d, 20d) }' as actual;

-- the box and circle matches
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (0d, 0d),(20d, 0d),(20d, 20d),(0d, 20d) }' as actual;

-- the polygon lies completely in the box and they have an intersection at the boundary line
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the polygon lies completely in the box and they have no intersection at the boundary
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (1d, 1d),(11d, 1d),(1d, 19d) }' as actual;

-- the box lies completely in the polygon and they have an intersection at the boundary line
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (10d , 10d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the box lies completely in the polygon and they have no intersection at the boundary
select 'sbox && spoly', 'f' as expected, sbox'((1d , 1d), (2d , 2d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the polygon partially lies in the box
select 'sbox && spoly', 't' as expected, sbox'((0d , 0d), (20d , 20d))' && spoly'{ (15d, 0d),(35d, 0d),(15d, 35d) }' as actual;

-- the box degenerated into the line lies in the boundary of polygon
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (20d , 0d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the box degenerated into the line lies in the interior of polygon
select 'sbox && spoly', 'f' as expected, sbox'((0d , 5d), (10d , 5d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the box degenerated into the point lies in the boundary of polygon
select 'sbox && spoly', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;

-- the box degenerated into the point lies in the interior of polygon
select 'sbox && spoly', 'f' as expected, sbox'((5d , 5d), (5d , 5d))' && spoly'{ (0d, 0d),(20d, 0d),(0d, 20d) }' as actual;


-- sbox vs sellipse


-- the box and ellipse have no common points
select 'sbox && sellipse', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sellipse'<{ 20d , 10d }, (0d, 90d) , 0d>' as actual;

-- the point of intersection of the box and ellipse is the boundary for both
select 'sbox && sellipse', 'f' as expected, sbox'((-10d , -10d), (10d , 10d))' && sellipse'<{ 80d , 10d }, (0d, 90d) , 90d>' as actual;

-- the ellipse lies completely in the box and they have an intersection at the boundary points
select 'sbox && sellipse', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sellipse'<{ 10d , 5d }, (10d, 10d) , 0d>' as actual;

-- the ellipse lies completely in the box and they have no intersection at the boundary points
select 'sbox && sellipse', 'f' as expected, sbox'((0d , 0d), (20d , 20d))' && sellipse'<{ 9d , 5d }, (10d, 10d) , 0d>' as actual;

-- the box lies completely in the ellipse and they have no intersection at the boundary points
select 'sbox && sellipse', 'f' as expected, sbox'((-10d , -10d), (10d , 10d))' && sellipse'<{ 30d , 20d }, (0d, 0d) , 0d>' as actual;

-- the ellipse partially lies in the box
select 'sbox && sellipse', 't' as expected, sbox'((-10d , -10d), (10d , 10d))' && sellipse'<{ 10d , 5d }, (10d, 10d) , 90d>' as actual;

-- the box degenerated into the point lies in the boundary of ellipse
select 'sbox && sellipse', 'f' as expected, sbox'((30d , 0d), (30d , 0d))' && sellipse'<{ 30d , 20d }, (0d, 0d) , 0d>' as actual;

-- the box degenerated into the point lies in the interior of ellipse
select 'sbox && sellipse', 'f' as expected, sbox'((0d , 0d), (10d , 0d))' && sellipse'<{ 30d , 20d }, (0d, 0d) , 0d>' as actual;

-- the box and ellipse are degenerated into the point and coincide
select 'sbox && sellipse', 'f' as expected, sbox'((0d , 0d), (0d , 0d))' && sellipse'<{ 0d , 0d }, (0d, 0d) , 0d>' as actual;

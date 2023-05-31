$dir = `cd`;
@data = split( /\\/, $dir );
$size = @data;
$name = @data[ $size - 1 ];

$/ = "\n";
chomp( $name );
$shortname = substr( $name, 0, 8 );

#`..\\..\\..\\build\\tools\\junction.exe .\\animations ..\\..\\..\\..\\Art\\Character-animations\\chars\\normal\\$name\\pure3d\\animations 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\child\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\fat\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\normal\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\old\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\woman\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;
`xcopy /Q /Y ..\\..\\..\\..\\Art\\Character-animations\\chars\\truckasaurus\\$name\\pure3d\\animations\\*.* .\\animations\\ 1>&2`;


print "#AUTO GENERATED FILE.  DO NOT EDIT BY HAND  Talk to Travis.\n";
print "#This is the ", $name, " choreo script.\n";
print "DEPEND ", $name, ".cho\n";
#print "DEPEND ", $name, "_npc.cho\n";
print "RESULT ", $name, ".cho chars\\\\",$shortname, ".cho\n";
#print "RESULT ", $name, "_npc.cho chars\\\\",$shortname, ".npc\n";

`xcopy /Q /Y /R .\\animations\\*.cho .\\animations\\choreo\\ 1>&2`;
#`xcopy /Q /Y /R .\\animations\\$name_npc.cho .\\animations\\choreo\\ 1>&2`;
`ren .\\animations\\choreo\\$name.cho .\\animations\\choreo\\$shortname.cho 1>&2`;
echo on
..\tools\xboxsync\xboxsync ..\..\cd\xbox\ e:\srr2\ -r
XbCp -y Debug\*.xbe  XE:\srr2\
XbCp -y Tune\*.xbe  XE:\srr2\
XbCp -y PALDeb~1\*.xbe  XE:\srr2\
XbCp -y PALTun~1\*.xbe  XE:\srr2\
XbCp -y PALRel~1\*.xbe  XE:\srr2\

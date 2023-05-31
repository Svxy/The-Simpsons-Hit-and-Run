The Simpsons Hit & Run - FE PIPELINE
Author: Tony Chu


Things to check before attempting to run FE pipeline:

(i) Make sure your local path to //depot/game/libs/pure3d/tools/commandline/bin/ is included in your %PATH% environment variable.
(ii) Make sure your local path to //depot/game/libs/pure3d/lib/perl/bin/ is included in your %PATH% environment variable.
(iii) Make sure all the platform CD folders (i.e. //depot/game/cd/<platform>/) is in your P4 client spec.


Steps to follow for running the FE pipeline:

1. In P4, sync to //depot/game/exportart/frontend/.
2. Open up a command prompt to //depot/game/build/art/frontend/.
3. If you just type 'build', you will see the syntax for specifying various pipeline options. (Note, some have not yet been implemented completely, so they may not work as expected.)

4(a). To pipe dynamically loaded FE art (i.e. art files in //depot/game/exportart/frontend/dynaload/), type:

	build dynamic [compress]

	- use 'build dynamic:cars [compress]' to pipe cars only
	- use 'build dynamic:images [compress]' to pipe images only
	- specify 'compress' for final builds

4(b). To pipe Scrooby FE art (i.e. art files in //depot/game/exportart/frontend/scrooby/), type:

	build all [compress]

	- specify 'compress' for final builds

4(c). To pipe the text bible, type:

	build text

	- note that 4(b) will also pipe the text bible

5. When the pipe is completed, the corresponding FE art files in the CD folder(s) will have already been automatically checked out in P4 under your default client spec. You should now revert all unchanged files and check-in only the files that have changed.

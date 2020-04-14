# DikuMUD-Delta
An updated version of the original DikuMUD (alfa release) with the code cleaned up.

## Purpose
This codebase provides the opportunity to easily compile (without error or warning) and run (without random crashes) a vanilla version of DikuMUD.

While the original codebase may have very well be quite stable, unfortunately, various bits of code have naturally become deprecated over the past couple decades. Whether due to changes in the GCC compiler, or because of C libraries being updated over the years, it was necessary to update the codebase, to "reacquire" the original stability.

There is _absolutely_ **no new functionality** provided with this release. The intention was not to _improve_ anything. Rather, preserving the original behaviours and features was paramount.

## Instructions
       # cd src/
       # make all
       # cd ../
       # src/diku
  
You can copy the **diku** executable to the parent directory (from src/) and run directly. However, no matter what, the server must be run from the top level directory, so that the server can access the lib/ directory directly.

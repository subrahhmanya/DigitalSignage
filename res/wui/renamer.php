<?php
// Rename Folders to enable temporary welcome.
echo "Current Date/Time is: ";
echo date( 'd/m/Y - H:ia' );
echo "<br/><br/>";
echo "Modifying Step 1...";
rename( "/screen/boards", "/screen/boards-on" );
echo " [OK]<br />";
echo "Modifying Step 2...";
rename( "/screen/boards-ol", "/screen/boards" );
echo " [OK]<br />";
echo "Modifying Step 3...";
rename( "/screen/boards-on", "/screen/boards-ol" );
echo " [OK]<br /><br />Please press BACK in the Browser to go back and see if it worked!<br /><br />There is 1 bug at present, that any board which is removed (ie, the welcome one) doesn't actually get removed, in which case, press ESCAPE on the Keyboard on the PC running the Signage Software, and wait for it to reload (within 1 minute)<br /><br />Thanks!  Paul :)";
?>

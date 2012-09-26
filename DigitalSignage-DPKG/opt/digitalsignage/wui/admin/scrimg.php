<?php

// seed with microseconds
function make_seed()
{
  list($usec, $sec) = explode(' ', microtime());
  return (float) $sec + ((float) $usec * 100000);
}

$file = '/screen/SCRDUMP.bmp';

if (!file_exists($file)) {
	// File doesn't exist.
	$file = '/var/www/bfile';
}

// tells it its a gif image
Header("Content-type: image/bmp");

$last_modified_time = filemtime($file);
$etag = md5_file($file); 

// dates back a bit (so doesn't go in cache, so peeps actually see changes)
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
// ye be modified always (so dost go in cache, so peeps actually see changes)
#header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s", $last_modified_time)." GMT");
header("Etag: $etag"); 

// HTTP/1.1 - no (so doesn't go in cache, so peeps actually see changes)
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);

// HTTP/1.0 - no (so doesn't go in cache, so peeps actually see changes)
header("Pragma: no-cache"); 

readfile($file); 
exit;
?>


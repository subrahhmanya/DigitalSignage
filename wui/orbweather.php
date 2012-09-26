<?php
$config[ 'url' ]       = "http://uk.weather.com/weather/today-Stone-ST15"; // url of html to grab
$config[ 'start_tag' ] = '<div id="today_current">'; // where you want to start grabbing
$config[ 'end_tag' ]   = '<div id="current_bottom">'; // where you want to stop grabbing
$config[ 'show_tags' ] = 1; // do you want the tags to be shown when you show the html? 1 = yes, 0 = no
class grabber
{
	var $error = '';
	var $html = '';
	function grabhtml( $url, $start, $end )
	{
		$file = file_get_contents( $url );
		if ( $file ) {
			if ( preg_match_all( "#$start(.*?)$end#s", $file, $match ) ) {
				$this->html = $match;
			} //preg_match_all( "#$start(.*?)$end#s", $file, $match )
			else {
				$this->error = "Tags cannot be found.";
			}
		} //$file
		else {
			$this->error = "Site cannot be found!";
		}
	}
	function strip( $html, $show, $start, $end )
	{
		if ( !$show ) {
			$html = str_replace( $start, "", $html );
			$html = str_replace( $end, "", $html );
			return $html;
		} //!$show
		else {
			return $html;
		}
	}
}
$grab = new grabber;
$grab->grabhtml( $config[ 'url' ], $config[ 'start_tag' ], $config[ 'end_tag' ] );
echo $grab->error;
if ( !$grab->error ) {
	// We need to add the upper part of the HTML document.
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<!-- Set the radar style as hidden for all locations-->
<html>
<head>
<TITLE>Today's Weather</TITLE>
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="expires" content="0">
<link rel="stylesheet" type="text/css" href="main.css">
<link rel="stylesheet" type="text/css" href="today.css">
<STYLE TYPE="text/css">
<!--
BODY {margin: 0}
-->
</STYLE>
</head>
<body>
<?php
	foreach ( $grab->html[ 0 ] as $html ) {
		$htmlns = preg_replace( "/\\t/", "", $html );
		echo preg_replace( "/(^[\r\n]*|[\r\n]+)[\s\t]*[\r\n]+/", "\n", $htmlns );
	} //$grab->html[ 0 ] as $html
	// Cleanly close any open HTML Tags (we know what to shut)
?>
</div>
</div>
</body>
</html>
<?php
} //!$grab->error
?>

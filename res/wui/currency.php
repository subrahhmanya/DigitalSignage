<?php
$config[ 'url' ]       = "http://www.currencysolutions.co.uk/resources/live-rates"; // url of html to grab
$config[ 'start_tag' ] = "<table class='template_bordered' style='width: 100%'>"; // where you want to start grabbing
$config[ 'end_tag' ]   = '<td>Hong Kong Dollar</td>'; // where you want to stop grabbing
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
				$this->error = "";
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
<TITLE>Currency Exchange Rates</TITLE>
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
<div id="today_current" style="width:325px"><div style="margin:12px 0px 0px 6px; font-size: 14px;"><center><b>Current Exchange Rates to 1GBP</b></center>
<?php
	$counter = 0;
	foreach ( $grab->html[ 0 ] as $html ) {
		$counter = $counter + 1;
		$htmlns = preg_replace( "/\\t/", "", $html );
		$htmlns = preg_replace( "@<a(?:.*)>@isU", "", $htmlns );
		$htmlns = preg_replace( "@</a>@isU", "", $htmlns );
		$htmlns = preg_replace( "@<td>Hong Kong Dollar</td>@isU", "", $htmlns );
		$htmlns = preg_replace( "@<table class='template_bordered' style='width: 100%'>@isU", "<table align='center' class='template_bordered' style='width: 90%'>", $htmlns );
		echo preg_replace( "/(^[\r\n]*|[\r\n]+)[\s\t]*[\r\n]+/", "\n", $htmlns );
	} //$grab->html[ 0 ] as $html
	if ( $counter == 0 ) {
		echo "<center><br /><br />There was a problem getting the current Currency Exchange Rates.</center>";
	} //$tfirst == 0
	// Cleanly close any open HTML Tags (we know what to shut)
?>
</tr></tbody></table></div></div>
</body>
</html>
<?php
} //!$grab->error
?>

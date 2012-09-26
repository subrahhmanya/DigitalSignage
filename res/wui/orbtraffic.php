<?php
$config[ 'urlA' ]      = "http://www.bbc.co.uk/travelnews/stoke/roads/planned"; // url of html to grab
$config[ 'urlB' ]      = "http://www.bbc.co.uk/travelnews/stoke/roads/unplanned"; // url of html to grab
//$config['start_tag'] = '<!-- Item start. -->'; // where you want to start grabbing
//$config['end_tag']   = '<!-- Item end. -->'; // where you want to stop grabbing
$config[ 'start_tag' ] = '<!-- Item start. -->\\r\\n';
$config[ 'end_tag' ]   = '\\r\\n<!-- Item end. -->';
$config[ 'show_tags' ] = 0; // do you want the tags to be shown when you show the html? 1 = yes, 0 = no
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
			$this->error = "";
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
function get_string_between( $string, $start, $end )
{
	$string = " " . $string;
	$ini    = strpos( $string, $start );
	if ( $ini == 0 )
		return "";
	$ini += strlen( $start );
	$len = strpos( $string, $end, $ini ) - $ini;
	return substr( $string, $ini, $len );
}
$grabA = new grabber;
$grabA->grabhtml( $config[ 'urlA' ], $config[ 'start_tag' ], $config[ 'end_tag' ] );
$grabB = new grabber;
$grabB->grabhtml( $config[ 'urlB' ], $config[ 'start_tag' ], $config[ 'end_tag' ] );
echo $grabA->error;
echo $grabB->error;
if ( ( !$grabA->error ) && ( !$grabB->error ) ) {
	// We need to add the upper part of the HTML document.
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<!-- Set the radar style as hidden for all locations-->
<html>
<head>
<TITLE>Traffic Information</TITLE>
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="expires" content="0">
<link rel="stylesheet" type="text/css" href="newmain.css">
<!-- <link rel="stylesheet" type="text/css" href="today.css"> -->
<link rel="stylesheet" href="travel.css" type="text/css" /> 
<style type="text/css">
	.travel-nav.secondary{ display: none;} /* hide secondary nav */
	.tab-public{display: none;} /* hide public tabs */
	.travel-tabs-foot{width: 360px;}
	#counter2 span.counter_val{font-size: 12px; margin-top: 8px;} /* adjust size of jamcam count for large number */
</style>
<STYLE TYPE="text/css">
<!--
BODY {margin: 0}
-->
</STYLE>
</head>
<body>
<div id="today_current" style="width:509px;"><div style="margin:0px 0px 0px 0px"><br /><h1>&nbsp;</h1>
<?php
	$tAlerts = array( );
	$tfirst  = 0;
	foreach ( $grabA->html[ 0 ] as $html ) {
		$tfirst     = $tfirst + 1;
		$htmlns     = $html;
		$htmlns     = preg_replace( "@<a(?:.*)>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@</a>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@<span>Help</span>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@<span>Show On Map</span>@isU", "", $htmlns );
		$htmlns     = preg_replace( '@ even">@isU', '">', $htmlns );
		$htmlns     = preg_replace( '@ odd">@isU', '">', $htmlns );
		$htmlns     = preg_replace( '@<div class="item-incident incident">Roadworks</div>@isU', '<div class="item-incident roadworks">Roadworks</div>', $htmlns );
		$timestamp  = get_string_between( $htmlns, '<li id="item-', '" class="severity-' );
		$severity   = get_string_between( $htmlns, '" class="severity-', '">' );
		$tAlerts[ ] = array(
			 'content' => $htmlns,
			'timestamp' => $timestamp,
			'severity' => $severity 
		);
	} //$grabA->html[ 0 ] as $html
	foreach ( $grabB->html[ 0 ] as $html ) {
		$tfirst     = $tfirst + 1;
		$htmlns     = $html;
		$htmlns     = preg_replace( "@<a(?:.*)>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@</a>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@<span>Help</span>@isU", "", $htmlns );
		$htmlns     = preg_replace( "@<span>Show On Map</span>@isU", "", $htmlns );
		$htmlns     = preg_replace( '@ even">@isU', '">', $htmlns );
		$htmlns     = preg_replace( '@ odd">@isU', '">', $htmlns );
		$htmlns     = preg_replace( '@<div class="item-incident incident">Roadworks</div>@isU', '<div class="item-incident roadworks">Roadworks</div>', $htmlns );
		$timestamp  = get_string_between( $htmlns, '<li id="item-', '" class="severity-' );
		$severity   = get_string_between( $htmlns, '" class="severity-', '">' );
		$tAlerts[ ] = array(
			 'content' => $htmlns,
			'timestamp' => $timestamp,
			'severity' => $severity 
		);
	} //$grabB->html[ 0 ] as $html
	foreach ( $tAlerts as $c => $key ) {
		$sort_content[ ]   = $key[ 'content' ];
		$sort_timestamp[ ] = $key[ 'timestamp' ];
		$sort_severity[ ]  = $key[ 'severity' ];
	} //$tAlerts as $c => $key
	array_multisort( $sort_severity, SORT_DESC, $sort_timestamp, SORT_DESC, $tAlerts );
	$cS1 = 0;
	$cS2 = 0;
	$cS3 = 0;
	$cS4 = 0;
	$cS5 = 0;
	$cS6 = 0;
	foreach ( $tAlerts as $output ) {
		if ( ( $output[ 'severity' ] == 6 ) && ( $cS4 == 0 ) ) {
			echo '<div class="itemgroup"><div class="item-head severity-6"></div><ul>';
			$cS6 = 1;
		} //( $output[ 'severity' ] == 6 ) && ( $cS4 == 0 )
		if ( ( $output[ 'severity' ] == 5 ) && ( $cS4 == 0 ) ) {
			if ( $cS6 == 1 )
				echo "</ul></div>";
			echo '<div class="itemgroup"><div class="item-head severity-5"></div><ul>';
			$cS5 = 1;
		} //( $output[ 'severity' ] == 5 ) && ( $cS4 == 0 )
		if ( ( $output[ 'severity' ] == 4 ) && ( $cS4 == 0 ) ) {
			if ( ( $cS6 == 1 ) || ( $cS5 == 1 ) )
				echo "</ul></div>";
			echo '<div class="itemgroup"><div class="item-head severity-4">Severe</div><ul>';
			$cS4 = 1;
		} //( $output[ 'severity' ] == 4 ) && ( $cS4 == 0 )
		if ( ( $output[ 'severity' ] == 3 ) && ( $cS3 == 0 ) ) {
			if ( ( $cS6 == 1 ) || ( $cS5 == 1 ) || ( $cS4 == 1 ) )
				echo "</ul></div>";
			echo '<div class="itemgroup"><div class="item-head severity-3">Medium</div><ul>';
			$cS3 = 1;
		} //( $output[ 'severity' ] == 3 ) && ( $cS3 == 0 )
		if ( ( $output[ 'severity' ] == 2 ) && ( $cS2 == 0 ) ) {
			if ( ( $cS6 == 1 ) || ( $cS5 == 1 ) || ( $cS4 == 1 ) || ( $cS3 == 1 ) )
				echo "</ul></div>";
			echo '<div class="itemgroup"><div class="item-head severity-2">Slight</div><ul>';
			$cS2 = 1;
		} //( $output[ 'severity' ] == 2 ) && ( $cS2 == 0 )
		if ( ( $output[ 'severity' ] == 1 ) && ( $cS1 == 0 ) ) {
			if ( ( $cS6 == 1 ) || ( $cS5 == 1 ) || ( $cS4 == 1 ) || ( $cS3 == 1 ) || ( $cS2 == 1 ) )
				echo "</ul></div>";
			echo '<div class="itemgroup"><div class="item-head severity-1">Very Slight</div><ul>';
			$cS1 = 1;
		} //( $output[ 'severity' ] == 1 ) && ( $cS1 == 0 )
		echo $output[ 'content' ];
	} //$tAlerts as $output
	// Did we report anything?  Check if $tfirst is 0, else display "No Traffic"
	if ( $tfirst == 0 ) {
		echo "<center><br /><br />There is currently no local traffic alerts in effect.</center>";
	} //$tfirst == 0
	else {
		echo "</ul>";
	}
?>
</div>
<br />
</body>
</html>
<?php
} //( !$grabA->error ) && ( !$grabB->error )
?>

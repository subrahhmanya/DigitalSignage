<?php
// Display Host and IP
$ip_address = $_SERVER[ 'SERVER_ADDR' ];
if ( $ip_address == NULL ) {
	$ip_address = $_SERVER[ 'HTTP_X_FORWARDED_FOR' ];
} //$ip_address == NULL
$host_string = gethostname() . " (" . $ip_address . ")";
?>
<html>
<head>
<TITLE>Orbital Digital Signage System</TITLE>
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="expires" content="0">
<script type='text/javascript' src='admin/includes/jquery.js'></script>
<link rel="stylesheet" type="text/css" href="main.css">
<link rel="stylesheet" type="text/css" href="today.css">
<STYLE TYPE="text/css">
<!--
BODY {margin: 0; background-color:#000000; width: 100%; height: 100%;}

div.fadehover {
	position: relative;
	}
 
img.a {
	position: absolute;
	left: 0;
	top: 0;
        z-index: 10;
	}
 
img.b {
	position: absolute;
	left: 0;
	top: 0;
	}
-->
</STYLE>
<script type="text/javascript">
//<![CDATA[
window.onload = function() {
	currTimer=0;
	maxTimer=30;
	setInterval("doCounter()", 1000);
}

function doCounter() {
	currTimer++;
	if (currTimer > maxTimer) {
		currTimer=0;
		doPicRefresh();
	}
	tTimer = document.getElementById('cTimer');
	tTimer.innerHTML = maxTimer - currTimer;
}

function doPicRefresh() {
	img = document.getElementById('refreshImage');
	img.src += "?";
	img.src=img.src.replace(img);
}
//]]>
</script>
<script type='text/javascript'>
$(document).ready(function(){
    $("img.a").hover(

    function() {
        $(this).stop().animate({"opacity": "0.6"}, "slow");
    },

    function() {
        $(this).stop().animate({"opacity": "0"}, "slow");
    });
});
</script>
</head>
<body>
<center>
<br /><br />
<div style="position: relative; left: 0; top: 0; width: 850; height: 544; background-image: url(admin/images/tvbg.png); ">
    <div style="position: absolute; top: -5; left: 0; width: 100%" ><h1><center><font color="#ffffff">Orbital Digital Signage System</font></center></h1></div>
    <div style="position: absolute; bottom: 53; right: 92;"><a href="renamer.php">
    <div class="fadehover">
        <img border="0" style="filter: alpha(opacity=0); -moz-opacity: 0; -khtml-opacity: 0; opacity: 0; border: none;" src="admin/images/dcfg.png" width="32" height="32" alt="" title="Configure This Screen" class="a"/>
    </div></a></div>
    <div style="position: relative; top: 38; left: 0;"><img src="admin/scrimg.php" width="770" height="435" id="refreshImage" /></div>
    <div style="position: relative; top: 49; left: 0;" ><b><font color="#ffffff"><?php
echo $host_string;
?></font></b></div>
    <div style="position: relative; top: 67; left: 0;" ><font color="#ffffff">Live Preview will update in <font id="cTimer">30</font> Seconds</font></div>
</div>
</center>
</body>
</html>

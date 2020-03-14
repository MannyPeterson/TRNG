<?php
if(!isset($_GET['q'])) {
	echo "ERROR 001";
	exit;
}
if(!ctype_digit($_GET['q'])) {
	echo "ERROR 002";
	exit;
}
$q = (int)$_GET['q'];
if($q < 1) {
	echo "ERROR 003";
	exit;
}
if($q > 5) {
	echo "ERROR 004";
	exit;
}
$filename = "tmp/" . strtoupper(uniqid("CHUNK", true)) . ".txt";
$dbhost = '127.0.0.1:3306';
$dbuser = 'TRNG';
$dbpass = 'TRNGTRNG';
$dbname = 'TRNG';
$conn = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
if($conn->connect_error ) {
	echo "ERROR 005";
	exit;
}
$sql = "SELECT chunk, created FROM chunks ORDER BY created ASC LIMIT " . $q;
$result = $conn->query($sql);
$conn->close();
if ($result->num_rows > 0) {
	$file = fopen($filename, "w");
	while($row = $result->fetch_assoc()) {
		fwrite($file, $row["chunk"] . PHP_EOL);
	}
	fclose($file);
	$file = fopen($filename, "r");
	header("Content-Disposition: attachment; filename=\"" . basename($filename) . "\"");
	header("Content-Type: application/octet-stream");
	header("Content-Length: " . filesize($filename));
	header("Connection: close");
	fpassthru($file);
	fclose($file);
	unlink($filename);
	exit;
} else {
	echo "ERROR 006";
	exit;
}
exit;
?>

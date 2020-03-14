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
$filename = "tmp/" . strtoupper(uniqid("CHUNK", false)) . ".txt";
$dbhost = '127.0.0.1:3306';
$dbuser = 'TRNG';
$dbpass = 'TRNGTRNG';
$dbname = 'TRNG';
$conn = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
if($conn->connect_error ) {
	echo "ERROR 005";
	exit;
}
$sql = "SELECT client, chunks FROM clients WHERE client = '" . $_SERVER['REMOTE_ADDR'] . "'";
$result = $conn->query($sql);
if ($result->num_rows == 0) {
	$sql = "INSERT INTO clients (client, chunks) VALUES ('" . $_SERVER['REMOTE_ADDR'] . "', " . $q . ")";
	if($conn->query($sql) != TRUE) {
		echo "ERROR 006";
		$conn->close();
		exit;
	} 
} else {
	$row = $result->fetch_assoc();
	$chunks = (int)$row['chunks'];
	if($chunks > 10) {
		echo "You have exceeded your daily chunk limit.";
		$conn->close();
		exit;
	} else {
		$newchunks = $chunks + $q + 0;
		$sql = "UPDATE clients SET chunks = " . $newchunks . " WHERE client = '" . $_SERVER['REMOTE_ADDR'] . "'";
		if($conn->query($sql) != TRUE) {
			echo "ERROR 007";
			$conn->close();
			exit;
		}
	}
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
	echo "ERROR 008";
	exit;
}
exit;
?>

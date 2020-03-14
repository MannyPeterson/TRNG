<?php
if(!isset($_GET['request'])) {
	echo "E0001";
	exit;
}
if(!ctype_digit($_GET['request'])) {
	echo "E0002";
	exit;
}
$requested = (int)$_GET['request'];
if($requested < 1) {
	echo "E0003";
	exit;
}
if($requested > 5) {
	echo "E0004";
	exit;
}
$conn = new mysqli('127.0.0.1:3306', 'TRNG', 'TRNGTRNG', 'TRNG');
if($conn->connect_error ) {
	echo "E0005";
	exit;
}
$sql = "SELECT issued FROM clients WHERE client = '" . $_SERVER['REMOTE_ADDR'] . "'";
$result = $conn->query($sql);
if ($result->num_rows == 0) {
	$sql = "INSERT INTO clients (client, issued) VALUES ('" . $_SERVER['REMOTE_ADDR'] . "', " . $requested . ")";
	if($conn->query($sql) != TRUE) {
		echo "E0006";
		$conn->close();
		exit;
	} 
} else {
	$row = $result->fetch_assoc();
	$issued = (int)$row['issued'];
	if($issued > 10) {
		echo "E0007 (You have exceeded your daily chunk limit.)";
		$conn->close();
		exit;
	} else {
		$issued = $issued + $requested + 0;
		$sql = "UPDATE clients SET issued = " . $issued . " WHERE client = '" . $_SERVER['REMOTE_ADDR'] . "'";
		if($conn->query($sql) != TRUE) {
			echo "E0008";
			$conn->close();
			exit;
		}
	}
}
$sql = "SELECT chunk, created FROM chunks ORDER BY created ASC LIMIT " . $requested;
$result = $conn->query($sql);
if ($result->num_rows > 0) {
	$filename = "tmp/" . strtoupper(uniqid("CHUNK", false)) . ".txt";
	$file = fopen($filename, "w");
	while($row = $result->fetch_assoc()) {
		$chunk = $row["chunk"];
		fwrite($file, $chunk . PHP_EOL);
		$sql = "DELETE FROM chunks WHERE chunk = '" . $chunk . "'";
		if($conn->query($sql) != TRUE) {
			echo "E0009";
			$conn->close();
			exit;
		}
	}
	fclose($file);
	header("Content-Disposition: attachment; filename=\"" . basename($filename) . "\"");
	header("Content-Type: application/octet-stream");
	header("Content-Length: " . filesize($filename));
	header("Connection: close");
	$file = fopen($filename, "r");
	fpassthru($file);
	fclose($file);
	unlink($filename);
	$conn->close();
	exit;
} else {
	echo "E0010";
	$conn->close();
	exit;
}
exit;
?>

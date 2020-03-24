<html>
	<head>
		<title>Chess</title>
	</head>
<?php

echo "Start of PHP script<br>";

foreach($_POST as $key => $value) {
  echo "The HTML name: $key <br>";
  echo "The content of it: $value <br>";
}
if(isset($_POST["fen"])){
  echo "Inside isset <br>";

	$data_missing = array();

	if(empty($_POST["fen"])){
		$data_missing[] = "fen";
	}
	else{
		$fen= trim($_POST["fen"]);
	}

	if(empty($data_missing)){
		require_once("../PHPFiles/db_connection.php");

		$insert_statement = "INSERT INTO fen_hist (fen) VALUES";
		$values = "(\"" . $fen . "\")";
		$query = $insert_statement . $values;

    if (mysqli_query($conn, $query)){
      echo "FEN stored successfully!<br>";
    }
    else {
      echo "Error: " . $query . "<br>" . mysqli_error($conn);
    }
		mysqli_close($conn);
	}
	else {
		echo "Missing:<br/>";
		foreach($data_missing as $missing){
			echo $missing . "<br/>";
		}
	}
}

?>

	<body>
	</body>
</html>

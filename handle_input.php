<?php

$fp = fopen("./output.txt", "a");
fwrite($fp, date("h:i:sa") . "<br>");
fclose($fp);

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

		$insert_statement = "INSERT INTO fen_hist (fen, mtime) VALUES";
		#$values = "(\"" . $fen . "\")";
		$values = "(\"" . $fen . "\", CURTIME())";
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

foreach($_GET as $key => $value) {
  echo "The HTML name: $key <br>";
  echo "The content of it: $value <br>";
}
if($_SERVER['REQUEST_METHOD'] == "GET"){

	require_once("../PHPFiles/silent_db_connection.php");

	$select_statement = "SELECT fen FROM fen_hist ORDER BY mtime DESC LIMIT 1";
	$query = $select_statement;

	$result = mysqli_query($conn, $query);
	if (mysqli_num_rows($result) > 0) {
    // output data of each row
    while($row = mysqli_fetch_assoc($result)) {
			$data = ["fen" => $row["fen"]];
			header('Content-type:application/json;charset=utf-8');
      echo json_encode($data);
    }
	} else {
    echo "0 results";
	}
	mysqli_close($conn);

}

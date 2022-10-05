<?php
	$server = "localhost";
	$username = "root";
	$password = "";
	$dbname = "rss";
	$con = mysqli_connect("localhost", "root", "", "rss") or die("unable to connect");
?>

<!DOCTYPE html>
<html>
<head><meta http-equiv="refresh" content="3"><link rel="stylesheet" type="text/css" href="style.css" media="screen"/><link rel="apple-touch-icon" sizes="180x180" href="img/fav-icons/apple-touch-icon.png"/><link rel="icon" type="image/png" sizes="32x32" href="img/fav-icons/favicon-32x32.png"/><link rel="icon" type="image/png" sizes="16x16" href="img/fav-icons/favicon-16x16.png"/><meta name="theme-color" content="#6e7833"><title> R.S.S </title><body><br><h1><font color="navy">REPORTS SECURITY SYSTEM (NPA)</h1><h2><font color="white">RSS Data</font></h2>

<?php
	$query = " SELECT * from evidence ";
	$result = mysqli_query($con, $query);
	echo '  <table cellspacing = "5" cellpadding="5"><tr><th>ID</th><th>PHOTO</th><th>TIME</th><th>DATE</th></tr>';
	$result = mysqli_query($con, $query);
	
	if ($result->num_rows > 0) {
	    while ($row = mysqli_fetch_assoc($result)) { 
	    	$row_id = $row["id"];
	      	$row_photo = $row["photo"];
	      	$row_time = $row["date"];
	      	$row_date = $row["time"];
	        echo '	<tr><td>' . $row_id . '</td><td><img src="./images/' . $row["photo"] .'"></td><td>' . $row_time . '</td><td>' . $row_date . '</td></tr>';
		}
	}
?>
</table></body></html></body></html>

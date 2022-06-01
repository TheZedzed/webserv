<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="utf-8">
	<title>POST & PHP-CGI</title>
	<link rel="stylesheet" href="style.css">
	<style>
		input {
			margin: 10px;
		}
		button {
			padding: 5px;
			margin: 10px;
		}
		#form{
			text-align: center;
			width: 50%;
			margin: auto;
			background-color: lightgrey;
			border-radius: 6px;
			box-shadow: 3px 3px white;
		}
	</style>
</head>
<body style="background-color: #1F212E; font-family: Gill Sans, sans-serif;">
	<div>
		<h1 style="margin: 5px 0px -18px 0px; color: #00babc">Simple form to test POST method and PHP-CGI</h1>
	</div>
	<p style="margin-left:15px; font-style:italic; color:white;">
		Submit's the inputs and get's the sent data printed on self page.
	</p>
	<div id="form">
		<form method="post" action="submit.php">
			<input name="feild_one" placeholder="Input feild one...."></br>
			<input name="feild_two" placeholder="Input feild two...."></br>
			<button type="submit">Submit</button></br>
		</form>
	</div>
	<?php
		if ($_SERVER["REQUEST_METHOD"] == "POST") {
			print_r($_POST);
			print_r($_ENV);
		}
		if (isset($_POST['feild_one']) && isset($_POST['feild_two'])) {
			echo("Feild one = " . htmlspecialchars($_POST['feild_one']));
			echo("Feild two = " . htmlspecialchars($_POST['feild_two']));
			//setcookie(string "CooKi3");
		}
		if (isset($_COOKIE['cookie_value']))
			echo("Cookie value = " . htmlspecialchars($_COOKIE['cookie_value']))
	?>
	<p style="margin-top:15px; font-style:italic; text-align: center; color: darkgray;">
		Webserv 42 - 2022
	</p>
</body>
</html>

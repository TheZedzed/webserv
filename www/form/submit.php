<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="utf-8">
	<title>POST & PHP-CGI</title>
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
		h1 {
			color: white;
		}
		#cookie {
			font-size: 42px;

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
			<input name="field_one" placeholder="Input field one...."></br>
			<input name="field_two" placeholder="Input field two...."></br>
			<input name="field_three" placeholder="Enter cookie value...."></br>
			<button type="submit">Submit</button></br>
		</form>
	</div>
	<?php if (isset($_POST['field_one']) && isset($_POST['field_two'])) { ?>
	<h1>
	<?php 	echo("field one = " . htmlspecialchars($_POST['field_one'])); ?>
	</h1>
	<h1>
	<?php	echo("field two = " . htmlspecialchars($_POST['field_two'])); 
			setcookie("C00K13", htmlspecialchars($_POST['field_three']), time()+30);}?>
	</h1>
	<div id="cookie">
	</div>
	<p style="margin-top:15px; font-style:italic; text-align: center; color: darkgray;">
		Webserv 42 - 2022
	</p>
</body>
<script>
	var cook = document.getElementById('cookie');
	var cookies = document.cookie;
	console.log(cookies);
	if (!cookies)
		cook.textContent = "There are no cookies set.";	
	else
		cook.textContent = document.cookie;
</script>
</html>
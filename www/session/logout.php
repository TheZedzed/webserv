<?php
   session_start();
   unset($_SESSION["username"]);
   unset($_SESSION["password"]);
   unset($_SESSION['valid']);
   setcookie('phpsessid','value',time()-1);
   session_destroy();


   echo 'You have cleaned session';
   header('Refresh: 2; URL = login.php');
?>

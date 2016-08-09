<?php
header("Content-Type: text/html;charset=UTF-8");
header("Cache-Control: max-age=0");
error_reporting(E_ALL);
ini_set("display_errors", 1);
$sortie=0;
$question="";
$reponse="";
$sortie=0;
$lang="FR";
$botname="";
$bot_id="";
$row_cnt="";
$type="";
$action="nothing";
$question_id="";
$msg_read="";
	
$bdd = mysqli_connect('localhost', 'myai_shared', 'myai_shared*', 'shared_memory_FR');
mysqli_set_charset($bdd,"utf8");




if (isset($_GET["botname"]))
	{
	if ($_GET["botname"]<>"")
		{
		$botname=strtolower(urldecode($_GET["botname"]));
		}
	}
	
if (isset($_GET["bot_id"]))
	{
	if ($_GET["bot_id"]<>"")
		{
		$bot_id=$_GET["bot_id"];
		}
	}
 

if (isset($_GET["question"]))
	{
	if ($_GET["question"]<>"")
		{
		$question=strtolower(urldecode($_GET["question"]));
		}
	}
	
if (isset($_GET["action"]))
	{
	if ($_GET["action"]<>"")
		{
		$action=$_GET["action"];
		}
	}

if (isset($_GET["reponse"]))
	{
	if ($_GET["reponse"]<>"")
		{
		$reponse=strtolower(urldecode($_GET["reponse"]));
		}
	}
//echo $reponse;
if ($question<>"")
{
$resultat = mysqli_query($bdd, 'SELECT reponses.text,question_id FROM reponses INNER JOIN questions ON reponses.question_id = questions.id WHERE questions.text LIKE "%'.$question.'%"');

if ($resultat)
		{
		$row_cnt = $resultat->num_rows;	
		while($donnees = mysqli_fetch_assoc($resultat))
			{
			$sortie=$donnees['text'];
			$question_id=$donnees['question_id'];
			}
		}
		if ($row_cnt==0)
		{
			$sortie=0;
			
			
		}
}



if ($action=="GetUnreadMessageNumbers"&&$bot_id<>"")
{
$resultat = mysqli_query($bdd, 'SELECT message FROM messages WHERE etat=1 and robot_id="'.$bot_id.'"');
//echo 'SELECT message FROM messages WHERE etat=1 and robot_id="'.$bot_id.'"';
if ($resultat)
	{
	$sortie = $resultat->num_rows;	
	}
}



if ($action=="GetMessage"&&$bot_id<>"")
{
$resultat = mysqli_query($bdd, 'SELECT id,message,from_robot_name FROM messages WHERE etat=1 and robot_id="'.$bot_id.'" limit 1');

if ($resultat)
	{
	$row_cnt = $resultat->num_rows;
		while($donnees = mysqli_fetch_assoc($resultat))
			{
			$sortie=$donnees['from_robot_name']." , ".$donnees['message'];
			mysqli_query($bdd, 'UPDATE messages SET etat=0 WHERE id="'.$donnees['id'].'"');
			}
	}
}

if ($action=="NewMessage"&&$botname<>""&&$bot_id<>""&&$question<>"")
{
mysqli_query($bdd,"INSERT INTO messages (message,robot_id,from_robot_name,etat) VALUES('".$question."','".$bot_id."','".$botname."',1)");
//echo "INSERT INTO messages (message,robot_id,from_robot_name,etat) VALUES('".$question."','".$bot_id."','".$botname."',1)";
if ($resultat)
	{
	$sortie="OKMESSAGE";
	}
}



if ($action=="CheckRobot"&&$botname<>"")
{
$resultat = mysqli_query($bdd, 'SELECT * FROM robot_name WHERE name="'.$botname.'"');
$sortie="NOROBOTNAME";
if ($resultat)
	{
	$row_cnt = $resultat->num_rows;
	if ($row_cnt==0)
		{
		$sortie="NOROBOTNAME";
		}
		else
		{
			while($donnees = mysqli_fetch_assoc($resultat))
			{
			$bot_id=$donnees['bot_id'];
			}
		$sortie="OKROBOTNAME ".$bot_id;
		}	
	}
}


if ($action=="UpdateBotName")
{



$resultat = mysqli_query($bdd, 'SELECT * FROM robot_name WHERE name="'.$botname.'"');

if ($resultat)
		{
		
		if (($resultat->num_rows)>0) 
			{
			mysqli_query($bdd, 'UPDATE robot_name SET bot_id="'.$bot_id.'" WHERE name="'.$botname.'"');
			$sortie=3;
			}
		else
			{
			mysqli_query($bdd, 'REPLACE into robot_name (bot_id, name) values("'.$bot_id.'","'.$botname.'")');
			$sortie=4;
			}
		}




}



if ($question<>""&&$reponse<>""&&$action=="update")
{


	if ($question_id<>"")
		{
		mysqli_query($bdd,"UPDATE reponses SET text='".strtolower($reponse)."' WHERE question_id=".$question_id);	
		$sortie=2;	
		}
		else
		{
		mysqli_query($bdd,"INSERT INTO questions (text) VALUES('".$question."')");	
		$last_id = $bdd->insert_id;
		mysqli_query($bdd,"INSERT INTO reponses (text,question_id) VALUES('".$reponse."',".$last_id.")");
		$sortie=1;	
		}
}






echo $sortie;

?>
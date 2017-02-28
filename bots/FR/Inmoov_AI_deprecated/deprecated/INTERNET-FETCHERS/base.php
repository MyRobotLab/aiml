<?php
header("Content-Type: text/html;charset=UTF-8");
header("Cache-Control: max-age=0");
error_reporting(E_ALL);
ini_set("display_errors", 1);
include 'phpdom_dependencies.php';
$sortie=0;
$pic="none_xyz";
$lang="FR";
$CITY="PARIS";
$units="metric";
$genre="RANDOM";
$row_cnt=0;
$type="";

$bdd = mysqli_connect('localhost', '', 'password', '');
mysqli_set_charset($bdd,"utf8");

if (isset($_GET["type"]))
	{
	if ($_GET["type"]<>"")
		{
		$type=$_GET["type"];
		}
	}
if (isset($_GET["genre"]))
	{
	if ($_GET["genre"]<>"")
		{
		$genre=$_GET["genre"];
		}
	}
if (isset($_GET["pic"]))
	{
	if ($_GET["pic"]<>"")
		{
		$pic=$_GET["pic"];
		}
	}
if (isset($_GET["lang"]))
	{
	if ($_GET["lang"]<>"")
		{
		$lang= strtoupper($_GET["lang"]);
		}
	}
if (isset($_GET["CITY"]))
	{
	if ($_GET["CITY"]<>"")
		{
		$CITY=$_GET["CITY"];
		}
	}	
if (isset($_GET["units"]))
	{
	if ($_GET["units"]<>"")
		{
		$units=$_GET["units"];
		}
	}	
if (isset($_GET["question"]))
	{
	if ($_GET["question"]<>"")
		{
		$question=$_GET["question"];
		}
	}
	

	
	
switch ($type){
	
include 'pics.php';
include 'weather.php';
include 'y_awsers.php';	

	
}
	
	
	
function remplaceGuillemets($texte)
{
    //Places chaque caractères du texte dans un tableau
    $listeCaracteres = str_split($texte, 1);
 
    //Créé un tableau contenant la position de chaque guillemets
    $rechercheGuillemets = array_keys($listeCaracteres, '"');
 
    //Parcours le tableau contenant la position des guillemets
    foreach ($rechercheGuillemets as $key => $value)
    {
        //Remplace alternativement par des guillemets ouvrante ou fermante
        if($key%2 == 0){ $listeCaracteres[$value] = '« '; }
        else{ $listeCaracteres[$value] = ' »'; }
    }
 
    //Reconstitue le texte
    return implode('', $listeCaracteres);
}

function check_urlimage($urlimage,$picV,$langV,$conn)
{
$UrlTmp=$urlimage;	
if (@getimagesize($urlimage))
{
	
}
else
{
$UrlTmp=file_get_contents('http://bot1.myai.cloud:9090/?pic='.urlencode ($picV));
}
updatecache($picV,$UrlTmp,$langV,$conn);	
return $UrlTmp;
}

function updatecache($nomV,$urlV,$langV,$conn)
{
if ($urlV<>""&&$urlV<>"0")
{
mysqli_query($conn,"INSERT INTO pic_fr (nom, url, lang) VALUES('".$nomV."','".$urlV."','".$langV."') ON DUPLICATE KEY UPDATE nom='".$nomV."', url='".$urlV."' , lang='".$langV."'");	
}
}




echo $sortie;
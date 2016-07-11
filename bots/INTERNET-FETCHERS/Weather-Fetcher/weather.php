case "meteo":
	
if($bdd)
	{
	$city="paris";
	$texte="Meteo problem";
	$url="http://api.openweathermap.org/data/2.5/forecast?q=".$city."&lang=".$lang."&units=".$units."&cnt=9&APPID=YOUR_KEY";
	$json=file_get_contents($url);
	$data=json_decode($json,true);
	$vitesse=intval($data['list'][0]['wind']['speed'])*3.6;
	if ($units=="metric")
	{
	$textMetric=" Kilomètres heures";	
	}
	else
	{
	$vitesse=$vitesse/0.621371;
	$textMetric=" Miles hour";
	}
	$vitesse=round($vitesse, 0, PHP_ROUND_HALF_UP);
	if ($lang=="FR")
	{	
	$texte="En ce moment, Le temps est ".$data['list'][0]['weather'][0]['description']." et il fait ".round($data['list'][0]['main']['temp'], 0, PHP_ROUND_HALF_UP)." degrés, et la vitesse du vent est de ".$vitesse.$textMetric;
	$texte=$texte." Demain, à la même heure, le temps sera ".$data['list'][8]['weather'][0]['description']." , et il fera ".round($data['list'][8]['main']['temp'], 0, PHP_ROUND_HALF_UP)." degrés";
	}
	else
	{
	$texte="Right now time is ".$data['list'][0]['weather'][0]['description']." and it's ".round($data['list'][0]['main']['temp'], 0, PHP_ROUND_HALF_UP)." degrés, and wind speed is ".$vitesse.$textMetric;
	$texte=$texte." , Tomorrow at the same time, the time will be , ".$data['list'][8]['weather'][0]['description']." and will be, ".round($data['list'][8]['main']['temp'], 0, PHP_ROUND_HALF_UP)." degrés";
	
	}
	$sortie=$texte;
	}

break;

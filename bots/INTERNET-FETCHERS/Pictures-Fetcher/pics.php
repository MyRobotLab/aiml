switch ($type){
	case "pic":
	
if($pic<>"none_xyz"&&$bdd)
	{
	$resultat = mysqli_query($bdd, 'SELECT * FROM pic_fr WHERE nom="'.$pic.'"');
	if ($resultat)
		{
		$row_cnt = $resultat->num_rows;	
		while($donnees = mysqli_fetch_assoc($resultat))
			{
			$sortie=$donnees['url'];
			}
		}
		if ($row_cnt==0)
		{
			$sortie=file_get_contents('http://bot1.myai.cloud:9090/?pic='.urlencode ($pic));
			$sortie=check_urlimage($sortie,$pic,$lang,$bdd);
			
		}
	}
$sortie=check_urlimage($sortie,$pic,$lang,$bdd);


break;

case "question":

$url='https://fr.answers.yahoo.com/';
if ($lang=="EN")
{
$url='https://answers.yahoo.com/';	
}

$html = file_get_html($url.'search/search_result?fr=uh3_answers_vert_gs&type=2button&p='.urlencode(utf8_encode($question)));
//echo $url.'search/search_result?fr=uh3_answers_vert_gs&type=2button&p='.urlencode(utf8_encode($question));
if (count($html->find('a[data-ylk*=slk:q;ql:]'))==0)
{
$reponse=0;	
}
else
{
$redir=$html->find('a[data-ylk*=slk:q;ql:]')[0]->href;
$html = file_get_html($url.$redir);
$redir=$html->find('span[class=ya-q-full-text]')[0]->plaintext;
$reponse=$redir;
}
$sortie=$reponse;


while (substr(strtok(substr($sortie, strpos($sortie,'http')), " "),0,4)=="http") {
//str_replace(strtok(substr($sortie, strpos($sortie,'http')), " "), " ",$sortie);
$sortie=str_replace(strtok(substr($sortie, strpos($sortie,'http')), " "), " ",$sortie);
$sortie=str_replace(strtok(substr($sortie, strpos($sortie,'http')), " "), " ",$sortie);

} 


while (substr(strtok(substr($sortie, strpos($sortie,'@')), " "),0,1)=="@") {
//str_replace(strtok(substr($sortie, strpos($sortie,'http')), " "), " ",$sortie);
$sortie=str_replace(strtok(substr($sortie, strpos($sortie,'@')), " "), " ",$sortie);
$sortie=str_replace(strtok(substr($sortie, strpos($sortie,'@')), " "), " ",$sortie);

} 


$sortie=str_replace('&quot;','',$sortie);
$sortie=html_entity_decode($sortie);


$sortie = str_replace("\t", '', $sortie); // remove tabs
$sortie = str_replace("\n", '', $sortie); // remove new lines
$sortie = str_replace("\r", '', $sortie); // remove carriage returns
$sortie = str_replace("       ", ' ', $sortie);
$sortie = str_replace("  ", ' ', $sortie);
//echo "000".substr(strtok(substr($sortie, strpos($sortie,'http')), " "),0,4)."***";
//echo "///".strtok(substr($sortie, strpos($sortie,'http')), " ")."///";
//strtok($value, " ")"000".
break;
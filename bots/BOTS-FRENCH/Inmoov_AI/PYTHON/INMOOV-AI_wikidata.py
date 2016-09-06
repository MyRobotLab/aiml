# -*- coding: utf-8 -*- 
	
def askWiki(query,question,retour): # retourne la description du sujet (query)
	#Light(1,0,0)
	query = unicode(query,'utf-8')# on force le format de police UTF-8 pour prendre en charge les accents
	if query[1]== "\'" : # Si le sujet contient un apostrophe , on efface tout ce qui est avant ! ( "l'ete" -> "ete")
		query2 = query[2:len(query)]
		query = query2
	print query # petit affichage de contrôle dans la console python ..
	word = wdf.cutStart(query) # on enleve le derminant ("le chat" -> "chat")
	start = wdf.grabStart(query) # on garde que le determinant ( je ne sais plus pourquoi j'ai eu besoin de ca, mais la fonction existe ...)
	wikiAnswer = wdf.getDescription(word) # recupere la description su wikidata
	answer = ( query + " est " + wikiAnswer)
	print wikiAnswer,answer
	if (wikiAnswer == "Not Found !") or (unicode(wikiAnswer[-9:],'utf-8') == u"Wikimedia") : # Si le document n'ai pas trouve , on reponds "je ne sais pas"
		QueryMemory(question,retour)
	else:
		talk(answer)
	#Light(1,1,1)
	
def WikiRaw(query): # retourne la description du sujet (query)
	#Light(1,0,0)
	query = unicode(query,'utf-8')# on force le format de police UTF-8 pour prendre en charge les accents
	if query[1]== "\'" : # Si le sujet contient un apostrophe , on efface tout ce qui est avant ! ( "l'ete" -> "ete")
		query2 = query[2:len(query)]
		query = query2
	print query # petit affichage de contrôle dans la console python ..
	word = wdf.cutStart(query) # on enleve le derminant ("le chat" -> "chat")
	start = wdf.grabStart(query) # on garde que le determinant ( je ne sais plus pourquoi j'ai eu besoin de ca, mais la fonction existe ...)
	wikiAnswer = wdf.getDescription(word) # recupere la description su wikidata
	answer = wikiAnswer
	if (wikiAnswer == "Not Found !") or (unicode(wikiAnswer[-9:],'utf-8') == u"Wikimedia") : # Si le document n'ai pas trouve , on reponds "je ne sais pas"
		chatBot.setPredicate("default","WikiRaw","0")
	else:
		chatBot.setPredicate("default","WikiRaw",answer)

def getProperty(query, what): # retourne la valeur contenue dans la propriete demandee (what)
	#Light(1,0,0)
	query = unicode(query,'utf-8')
	what = unicode(what,'utf-8')
	if query[1]== "\'" :
		query2 = query[2:len(query)]
		query = query2
	if what[1]== "\'" :
		what2 = what[2:len(what)]
		what = what2
		print "what = " + what + " - what2 = " + what2
	ID = "error"
	# le fichier propriete.txt contient les conversions propriete -> ID . wikidata n'utilise pas des mots mais des codes (monnaie -> P38)	f = codecs.open(unicode('os.getcwd().replace("develop", "").replace("\", "/") + "/proprietes_ID.txt','r',"utf-8") #
	f = codecs.open(oridir+WikiFile,'r','utf-8') #os.getcwd().replace("develop", "").replace("\\", "/") set you propertiesID.txt path
	
	for line in f:
    		line_textes=line.split(":")
    		if line_textes[0]== what:
	    		ID= line_textes[1]
	f.close()
	#print "query = " + query + " - what = " + what + " - ID = " + ID
	wikiAnswer= wdf.getData(query,ID) # recupere la valeur de la propriete si elle existe dans le document
	answer = ( what +" de " + query + " est " + wikiAnswer)
	
	if wikiAnswer == "Not Found !":
		answer=(question(query+" "+what))
		sleep(1);
		chatBot.getResponse(answer)
	else:
		talk(answer)
	#Light(1,1,1)
	return answer
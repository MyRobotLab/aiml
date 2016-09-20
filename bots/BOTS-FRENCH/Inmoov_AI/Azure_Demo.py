#AZURE DEMO


#voice service
Runtime.createAndStart("mouth", "AcapelaSpeech")
#azure service
AzureTranslator=Runtime.createAndStart("AzureTranslator", "AzureTranslator")
sleep(0.1)
#initiate azure
AzureTranslator.setCredentials("7da9defb-7d86-46e4-8607-5b82f280f430","IgCffuxo0QgHDzKMK1k5bf9/9GpP9ZwWwAaonacAcW0") # KEY and SECRET azure credentials
#voice output
mouth.setVoice("Ryan")
mouth.setLanguage("EN")
mouth.speak("This is a translate test")

#Origin language
supported_languages = { # as defined here: http://msdn.microsoft.com/en-us/library/hh456380.aspx
    'ar' : 'Arabic',
    'da' : 'Danish',
    'nl' : 'Dutch',
    'en' : 'English',
    'fr' : 'French',
    'de' : 'German',
    'el' : 'Greek',
    'it' : 'Italian',
    'no' : 'Norwegian',
    'es' : 'Spanish',
    'sv' : 'Swedish',
}

#acapela voice name correspondence
male_languages = { 
    'ar' : ' Nizar',
    'da' : 'Rasmus',
    'nl' : 'Jeroen',
    'en' : 'Ryan',
    'fr' : 'Antoine',
    'de' : 'Klaus',
    'el' : 'Dimitris',
    'it' : 'Vittorio',
    'no' : 'Olav',
    'es' : 'Antonio',
    'sv' : 'Emil',
	'ja' : 'Sakura',
}

#Translate to :
en_languages = {
    'arab' : 'ar',
	'arabe' : 'ar',
    'danish' : 'da',
    'dutch' : 'nl',
    'english' : 'en',
	'anglais' : 'en',
    'french' : 'fr',
    'german' : 'de',
	'allemand' : 'de',
    'greek' : 'el',
    'italian' : 'it',
	'italien' : 'it',
    'norway' : 'no',
    'spanish' : 'es',
    'espagnol' : 'es',
    'sweden' : 'sv',
	'japonais' : 'ja',
	
}


#main function
def translateText(text,language):
	
	AzureTranslator.detectLanguage(text)
	#AzureTranslator.fromLanguage('en')
	RealLang="0"
	try:
		RealLang=en_languages[language]
	except: 
		mouth.speak("There is a problem with azure, i am so sorry")
	print RealLang
	if RealLang!="0":
		AzureTranslator.toLanguage(RealLang)
		sleep(0.5)
		t_text=AzureTranslator.translate(text)   
		
		#small trick to prevent connection timeout :)
		i=0
		while 'Cannot find an active Azure Market Place' in t_text and i<50: 
			print(i,t_text)
			i += 1 
			sleep(0.2)
			AzureTranslator.detectLanguage(text)
			t_text=AzureTranslator.translate(text+" ")


		if 'Cannot find an active Azure Market Place' in t_text:
			mouth.speak("There is a problem with azure, i am so sorry")
		else:
			#change acapela voice
			mouth.setVoice(male_languages[RealLang])  
			mouth.speak(t_text)
			#Go back original acapela voice
			mouth.setVoice("Ryan")

# translateText(THE TEXT,TO LANGUAGE ( from #Translate to : )			
translateText("Hola buenos dias","french")
sleep(2)
translateText("Hello my name is Inmoov and I can translate so many languages ! ","italian")
sleep(2)
translateText("Coucou Gael, tu savais que je savais parler japonais aussi","japonais")


			
			


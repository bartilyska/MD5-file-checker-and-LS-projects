#!/bin/bash
# Autor           : Bartosz Łyskanowski
# Stworzony       : 5.05.2024
# Ostatnia modyfikacja przez : Bartosz Łyskanowski
# Ostatnia modyfikacja  : 12.05.2024 
# Wersja          : 1.0
# Opis     		  :
# Skrypt porównujący katalogi lub pliki  na podstawie ich skrótu md5. Dostepne sprawdzanie z poziomu 
# konsoli - za pomoca parametrow lub bez parametrow z wyborem plikow w zenity
#
# Na licencji Open Source - MIT License
# 
szukaj_pliki()
{
	plik1=$(zenity --file-selection )
	plik2=$(zenity --file-selection )
	if [[ -z $plik1 || -z $plik2 ]]
	then
		echo "Nie wybrano plików." 
 		exit
	fi
	if [[ $plik1 = $plik2 ]]
	then 
		czy_te_same="\nUWAGA porownujesz te same pliki!"	
	fi
	sum1=$(md5sum $plik1 | awk '{print $1}')
	sum2=$(md5sum $plik2 | awk '{print $1}')
	nazwa_plik1="${plik1##*/}" #wyciaga nazwe pliku z sciezki 
	nazwa_plik2="${plik2##*/}"
	if [[ $sum1 = $sum2 ]]
	then 
		pliki_sukces "$nazwa_plik1" "$nazwa_plik2"
	else 
		pliki_niepowodzenie "$nazwa_plik1" "$nazwa_plik2"
	fi
}
z_polecen_pliki()
{
	sum1=$(md5sum $1 | awk '{print $1}')
	sum2=$(md5sum $2 | awk '{print $1}')
	nazwa_plik1="${1##*/}"
	nazwa_plik2="${2##*/}"
	if [[ $1 = $2 ]]
	then 
		czy_te_same="\nUWAGA porownujesz te same pliki!"	
	fi
	if [[ $sum1 = $sum2 ]]
	then
		if [[ $nazwa_plik1 != $nazwa_plik2 ]] 
		then
			doklej="mimo innych nazw"
		fi
		echo -e "Zawartosc plikow $nazwa_plik1 oraz $nazwa_plik2 jest ta sama $doklej$czy_te_same"
	else 
		if [[ $nazwa_plik1 = $nazwa_plik2 ]] 
		then
			doklej="mimo tych samych nazw"
		fi
		echo "Pliki $nazwa_plik1 oraz $nazwa_plik2 maja rozna zawartosc $doklej"
	fi		
}
z_polecen_katalogi()
{
	cd $1 #wybiera pliki i wykonuje na nich md5sum + nazwy folderow i potem na calosci znow md5sum
	zawartosc1=$( (find . -type f -exec md5sum {} + ; find . -type d )  | LC_ALL=C sort | md5sum)
	cd $2 
	zawartosc2=$( (find . -type f -exec md5sum {} + ; find . -type d ) | LC_ALL=C sort | md5sum) 
	if [[ "${1: -1}" == "/" ]] #wycina / ze sciezki jezeli to ostatni znak
 	then
		nazwa_wytnij1="${1::-1}"
	else 
		nazwa_wytnij1="$1"
	fi

	if [[ "${2: -1}" == "/" ]]
 	then
		nazwa_wytnij2="${2::-1}"
	else 
		nazwa_wytnij2="$2"
	fi
	if [[ $nazwa_wytnij1 = $nazwa_wytnij2 ]]
	then 
		czy_te_same="\nUWAGA porownujesz te same katalogi!"	
	fi
	nazwa_katalog1="${nazwa_wytnij1##*/}" 
	nazwa_katalog2="${nazwa_wytnij2##*/}"
	if [[ $zawartosc1 = $zawartosc2 ]]
	then 
	    if [[ $nazwa_katalog1 != $nazwa_katalog2 ]]
		then
			doklej="mimo innych nazw"
		fi
		echo -e "Zawartosc katalogow $nazwa_katalog1 oraz $nazwa_katalog2 jest ta sama $doklej$czy_te_same"
	else 
	    if [[ $nazwa_katalog1 = $nazwa_katalog2 ]]
		then 
			doklej="mimo tych samych nazw"
		fi
		echo "Zawartosc katalogow $nazwa_katalog1 oraz $nazwa_katalog2 jest rozna $doklej" 
	fi
	
}
szukaj_katalogi()
{
	katalog1=$(zenity --file-selection --directory)
	katalog2=$(zenity --file-selection --directory)
	if [[ -z $katalog1 || -z $katalog2 ]]
	then
		echo "Nie wybrano katalogów." 
 		exit
	fi
	if [[ $katalog1 = $katalog2 ]]
	then 
		czy_te_same="\nUWAGA porownujesz te same katalogi!"	
	fi

	cd $katalog1 #obliczanie md5sum na podstawie wszystkich plikow i folderow w folderze
	zawartosc1=$( (find . -type f -exec md5sum {} + ; find . -type d ) | LC_ALL=C sort | md5sum)
	cd $katalog2 
	zawartosc2=$( (find . -type f -exec md5sum {} + ; find . -type d ) | LC_ALL=C sort | md5sum) 
	nazwa_katalog1="${katalog1##*/}"
	nazwa_katalog2="${katalog2##*/}"
	if [[ $zawartosc1 =  $zawartosc2 ]]
	then 
		katalogi_sukces	"$nazwa_katalog1" "$nazwa_katalog2"
	else 
		katalogi_niepowodzenie "$nazwa_katalog1" "$nazwa_katalog2" 
	fi
}
md5_z_polecen()
{
	if [[ ! -f "$1" || ! -f "$2" ]] #sprawdzenie czy argumenty to pliki czy foldery czy zly input
	then
 		if [[ -d "$1" && -d "$2" ]]
		then 
			z_polecen_katalogi "$1" "$2"
		else
			echo "Niepoprawne argumenty wywolania"
			exit
		fi
	else
		z_polecen_pliki "$1" "$2"	
	fi
}
pliki_sukces()
{
	if [[ $1 != $2 ]] 
	then
		doklej="mimo innych nazw"
	fi
	zenity --info --title "Sukces" --text "Pliki $1 oraz $2 mają tą samą zawartość\n$doklej$czy_te_same"
}
katalogi_sukces()
{
	if [[ $1 != $2 ]] 
	then
		doklej="mimo innych nazw"
	fi	
	zenity --info --title "Sukces" --text "Katalogi $1 oraz $2 mają tą samą zawartość\n$doklej$czy_te_same"
}
pliki_niepowodzenie()
{
	if [[ $1 = $2 ]] 
	then
		doklej="mimo tych samych nazw"
	fi	
	zenity --warning --title "Niepowodzenie" --width=200 --height=200 \
	 --text "Pliki $1 oraz $2 mają różną zawartość\n$doklej"
}
katalogi_niepowodzenie()
{
	if [[ $1 = $2 ]] 
	then
		doklej="mimo tych samych nazw"
	fi		
	zenity --warning --title "Niepowodzenie" --width=200 --height=200 \
	 --text "Katalogi $1 oraz $2 mają różną zawartość\n$doklej"
}
wypisz_pomoc()
{
	echo -e "Skrypt sluzy do porownywania plikow i katalogow na podstawie ich skrotu md5\nOpcje:\npomoc: -h\nwersja: -v\n\nUruchomienie skryptu bez parametrów i opcji uruchomi okno dialogowe w zenity.\nUżytkownik powinien wybrać interesującą go opcję i wybrać interesujące go pliki lub katalogi.\nPo wyborze otrzyma odpowiednią odpowiedź, czy podane pliki bądź katalogi są równe.\nNa nowo pojawi się okno dialogowe z wyborem opcji. Skrypt trwa do naciśnięcia "ZAKOŃCZ".\n\nUruchomienie skryptu z parametrami: należy podać dwie PEŁNE ścieżki do pliku lub katalogu.\nW przeciwnym wypadku skrypt poinformuje o niepoprawnych argumentach.\nScieżki można również podać po opcjach -hv.\nPo wpisaniu odpowiednich danych przychodzi odpowiedź o zgodności plików/katalogów.\nChcąc wykonać skrypt jeszcze raz trzeba go na nowo uruchomić.\n\nDziałanie: \nDla plików porównywany jest skrot md5 i ich nazwy.\nDla katalogów haszowane są nazwy wszystkich plikow w folderze i doklejone do tego nazwy podfolderow.\nLista ta jest sortowana a nastepnie na jej podstawie tworzony jest unikalny skrót md5, ktory jest porownywany.\n  "
}
pomoc_zenity()
{
	zenity --info --title "Pomoc" \
	--text "$(wypisz_pomoc)"
}
wypisz_wersje()
{
	echo "Wersja: 1.0"
	echo -e "Autor: Bartosz Łyskanowski s198051\n"
}
wersja_zenity()
{
	zenity --info --title "Wersja" \
	--text "$(wypisz_wersje)"
}
CZY_OPCJA=0
while getopts ":hv" OPT
do
	case $OPT in 
		h) 
			CZY_OPCJA=1
			wypisz_pomoc;;
		v)
			CZY_OPCJA=1	
			wypisz_wersje;;
			
		*)
			CZY_OPCJA=1
			echo "Nieznana opcja"
			exit ;;
	esac
done

shift $((OPTIND -1)) #zeby czytac parametry po np -hv argumenty

czy_te_same=""
doklej=""
if [[ "$#" -eq 2 ]]; then
		md5_z_polecen "$1" "$2"
elif [[ $CZY_OPCJA -eq 0 && "$#" -eq 0 ]] ; then	
		while [[ 0 ]] ; do
		doklej=""
		czy_te_same=""
		ID=$(zenity --list --height=400 --width=400  --title "Sprawdź pliki i foldery" \
    	--column="L.p" --column="Opcja" \
		1 "SPRAWDŹ PLIKI" \
   		2 "SPRAWDŹ KATALOGI" \
		3 "POMOC" \
		4 "WERSJA" \
   		5 "ZAKOŃCZ")
			case $ID in 
			1)
				szukaj_pliki;;
			2)
 				szukaj_katalogi;;
			3) 
				pomoc_zenity;;
			4)
				wersja_zenity;;
			5)
				exit;;
			esac
		done
elif [[ $CZY_OPCJA -eq 0 || ($CZY_OPCJA -eq 1 && ("$#" -ne 2 && "$#" -ne 0)) ]] ; then 
	echo "Zle podane argumenty"
fi

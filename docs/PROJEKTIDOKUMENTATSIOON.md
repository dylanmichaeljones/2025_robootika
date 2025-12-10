# Projekti dokumentatsioon

## 1. Projekti eesmärk ja seadme lühikirjeldus
**Mis asi see on, mida ja miks me teeme? Millist praktilist probleemi see lahendab?**

Meie projekti eesmärk on luua veekeetja, mille töö käib läbi tuumajaama stiilis kasutajaliidese. Arduiino sisaldab endas lihtsustatud tuumareaktori simulatsiooni ning energia tootmisel lülitab sisse reaalses tassis oleva veekeetja. Kasutaja kätes on tagada tuumareaktoris tasakaal tuumareaktsiooni, temperatuuri ja veetaseme üle. 
Sellega tuleb ka kaasa üldine tuumajaama simulatsioon, läbi mille veekeetja tööd juhtida.


---

## 2. Sisendite loetelu
**Millised on süsteemi poolt loetavad / mõõdetavad sisendid? Millega neid mõõdetakse / tuvastatakse?**

Peamised sisendid:
Kütusevarraste kontroll potentiomeetrid (3 tükki) - Mõõdetakse arduiino poolt ning määravad ära süsteemi.
Kontrollvarda potentiomeeter - Määrab kontrollvarda töö tugevust, millega kontrollida reaktsiooni kulgu.
Veepumpade potentiomeeter (2 tükki) - kontrollivad pumpade jõudu ning seekaudu palju vett reaktorisse siseneb. Kontrollitakse sellega reaktori temperatuuri ning ka veetaset.
Veepumpade master switch: arduiino väline, kontrollib elektrivoolu pumpade vooluringi.
Veepumpade lülitid(2 tükki): Lülitavad eri pumpade vooluringi.
Turbiini lüliti - Laseb genereeritud auru reaktorist turbiini tootes sellega energiat. 
Hoiatustulede lüliti - kontrollib hoiatustulede vooluringi.


---

## 3. Väljundite loetelu
**Mida süsteem teeb / muudab? Millega väljund realiseeritakse?**

Arduiino reaktor annab väljundina peamiselt ühe väärtuse ja see on energia. See muudetakse väärtusteks mille põhjal otsustatakse küttekeha kas sisse või välja lülitada.

Tähtsaimad väljundid on 1637 4 kohalised ekraanid, mis annavad kasutajatele infot reaktori seisundi koha pealt.
Peamised ekraanid on veetase, veetemperatuur, reaktoris toimuv reaktiivsus, genereeritud energia. 
Mingit küttekeha vee keetmise eesmärgil
Tugiekraanid(1 on seotud pumpadega ja näitavad pumpade hetkekiirust, duubeldades potentiomeetri näitu.
RGB riba kontrollvarraste juures muudavad eredust vastavalt kontrollvarraste sisestuse kogusügavusele.
Hoiatustuled(kollane LED): Madal veetase,
                          Kõrge temperatuur,
                          kõrge reaktiivsus
                          superhoiatus
7 ekraani eri simulatsiooniga seotud tulemuste näitamiseks. 
Suur RGB led strip juhtvarraste staatuse näitamiseks

---

## 4. Nõuded loodavale seadmele
**Mis peab toimuma, kui kasutaja teeb mingi toimingu? Kirjelda käitumisloogika.**

Kui reaktor on külm ja alles alustatud peab reaktor olema algväärtustatud sarnaselt:
veetemperatuur 18.0c,
Veetase 2000.0liitrit
Reaktiivsus 0.

Varraste sisestamisel üle piirväärtuse (100 ühikut 3069st (iga varras on 1023 bitti)_käivitub kickstart kus sisestatakse reaktorisse reaktiivsus ühikuga 5.

Reaktiivsus peab ajas tõusma ruutfunktsiooniga kütusevarraste reaktiivsus korda nende sisestusügavus kui kütusevardad on sisestatud. Reaktiivsus peab aeglaselt langema kui kütusevardad on reaktorist välja tõmmatud.

Kontrollvarras peab seda reaktsiioni tõusu aeglustama.

Reaktiivsus tõstab aeglaselt vee temperatuuri. Üle 70c hakkab vesi aurama ning sellega väheneb veetase ning genereeritakse auru.

Vesi jääb 100c ringi kus ta keeb ning auru toodetakse rohkem vastavalt reaktiivsusele ning vähendatakse ka veetaset. Keemine kaitseb vee kiiraurustumist kuni minimaalse veetasemeni.

Kui turbiin on sisse lülitatud genereerib turbiin energiat millega on võimalik hiljem liidestada füüsilises maailmas olev küttekeha.

Veepumbad lisavad reaktorisse vett ning jahutavad veetemperatuuri.

Kui reaktiivsus ja temperatuur lähvad liiga kõrgeks ei ole võimalik reaktorit enam kontrollida ja see ""sulab üles""


---

## 5. Süsteemi füüsiliste komponentide loetelu
**Millest seade koosneb? Lisa lingid või täpsed nimed, et keegi teine saaks sama asja uuesti osta / teha.**

Tabelina või punktidena. Nt:

- Arduino Uno (mikrokontroller)
- 3 slide potentiomeetrit
- 2 keerdpotentiomeetrit
- Vana vene veekeetja
- relee veekeetjale
- RGB ledstrip
- 16 ledi seisundi ja hoiatustulede jaoks
- 4 2 seisundilist lülitit
- pisoelektirline heligeneraator
---

## 6. Ühendusskeem
**Kuidas kõik osad on omavahel ühendatud?**

Wokwi simulaatoris ehitatud mudel, mis näitab ka ühendusi.
https://wokwi.com/projects/448691473688261633

- Fail `hardware/wiring-diagram.png` peab näitama vähemalt:
  - milline pin Arduinol läheb millise komponendi sisendisse,
  - kuidas on toide ühendatud.


---

## 7. Süsteemi juhtiv kood (või pseudokood)
https://wokwi.com/projects/448691473688261633

**Kirjelda programmi loogikat nii, et seda on võimalik aru saada ka hiljem.**  
Programm on jaotatud osadeks, need osad on:
-konstantide ja muutujate määramine
Määratakse ära erinevad konstandid ja muutujad mis on globaalselt saadavad koodis. Nendega saab kergelt tuunida reaktori käitumist ja tundlikust.
-void setup
Initsialiseerib ekraanid ja seriali
-void loop
Seab üles ajamõõtmise kus ta uuendab iga sekund arvutusi ning kutsub esile allpool loeteldud funktsioonid.
-inputtide funktsioon
Loeb hetkeseisu eri sisenditest ning uuendab seotud muutujaid vastavalt nendele.
-reaktori funktsioon
Arvutab välja hetkereaktiivsuse ruutfunktsiooni sõltuvalt sisenditele. Lisaks muudab selle reaktiivsuse soojuseks.
-turbiini funktsioon
Võtab soojuse ning muudab selle energiaks. Lisaks ka arvutab veetaset ning veekadu ning määrab ära ka pumpade töö.
-info näitamine
Näitab infot ekraanidel muutes nad esmalt integerideks.
-serialisse info printimine
Trükib kõik vajaliku info ka serialisse.

`src/reaktorScript.ino`.  


dududu

# Projekti dokumentatsioon

## 1. Projekti eesmärk ja seadme lühikirjeldus
**Mis asi see on, mida ja miks me teeme? Millist praktilist probleemi see lahendab?**

Näide stiilist:
- Meie projekti eesmärk on luua ventilaator, mida saab juhtida infrapuna puldiga.
- Seadet saab kasutada näiteks ruumi jahutamiseks olukorras, kus käega lülitile ulatamine on ebamugav.
- Peamised komponendid: elektrimootor, servo, ventilaatori labad (3D prinditud), Arduino mikrokontroller.

👉 _Asenda see kirjeldus enda seadme kirjeldusega._

---

## 2. Sisendite loetelu
**Millised on süsteemi poolt loetavad / mõõdetavad sisendid? Millega neid mõõdetakse / tuvastatakse?**

Kirjelda kõik sisendid eraldi punktidena.  
Näited (asenda enda projektiga):

- Nupp "vasakule" puldil → IR-sensor loeb signaali
- Nupp "paremale" puldil → IR-sensor loeb signaali
- Nupp "+" puldil → IR-sensor loeb signaali (tõsta kiirust)
- Nupp "-" puldil → IR-sensor loeb signaali (vähenda kiirust)
- ON/OFF nupp → IR-sensor loeb signaali

👉 _Kui sinu süsteem kasutab muid sensoreid (ultraheli, temperatuuriandur, valgusandur, joystick, BLE telefonis vms), kirjelda need siin koos füüsilise sisendi allikaga._

---

## 3. Väljundite loetelu
**Mida süsteem teeb / muudab? Millega väljund realiseeritakse?**

Näited (asenda enda projektiga):
- Ventilaator pöörleb kiiremini / aeglasemalt → DC mootor
- Ventilaator suunab õhu vasakule / paremale → servo
- LED süttib / kustub → LED
- Ekraanile kuvatakse temperatuur → OLED ekraan

---

## 4. Nõuded loodavale seadmele
**Mis peab toimuma, kui kasutaja teeb mingi toimingu? Kirjelda käitumisloogika.**

Kirjuta reeglid kujul "Kui X, siis Y".  
Näited (kohanda enda projektile):

- Kui vajutatakse ON/OFF nuppu, siis:
  - kui ventilaator on väljas → ventilaator lülitub sisse keskmise kiirusega;
  - kui ventilaator töötab → ventilaator pöördub keskasendisse ja lülitub välja.

- Kui vajutatakse vasak/noole nuppu, liigub ventilaatori pea iga vajutusega X kraadi vasakule, kuni vasak piir on käes. Kui piir käes, siis rohkem ei liigu.

- Kui ventilaator töötab maksimumkiirusel ja vajutatakse "+" → kiirus ei suurene enam.

👉 _Pane siia KÕIK kokkulepitud reeglid. Need reeglid on alus, mille järgi hiljem hinnatakse, kas teie lahendus vastab eesmärgile._

---

## 5. Süsteemi füüsiliste komponentide loetelu
**Millest seade koosneb? Lisa lingid või täpsed nimed, et keegi teine saaks sama asja uuesti osta / teha.**

Tabelina või punktidena. Nt:

- Arduino Uno (mikrokontroller)
- IR-vastuvõtja + pult (tüüp: XY123)  
- Väike elektrimootor (DC, ___ V)
- Mootoridraiver (L298N vms)
- Servo (mudel: SG90 / MG90S / muu)
- 3D-prinditud ventilaatori labad (STL-failid lisage kataloogi `hardware/`)
- Toiteallikas (___ V / ___ A)

👉 _Kui ise tegite 3D mudeli, lisage STL või Fusion faili `hardware/` alla. Kui kasutasite netist leitud mudelit, märkige allikas._

---

## 6. Ühendusskeem
**Kuidas kõik osad on omavahel ühendatud?**

- Lisa siia pilt või skeemi kirjeldus.
- Fail `hardware/wiring-diagram.png` peab näitama vähemalt:
  - milline pin Arduinol läheb millise komponendi sisendisse,
  - kuidas on toide ühendatud.

Kui skeemi pole veel joonistatud, siis vähemalt kirjelda tekstina, nt:

- IR-sensor OUT → Arduino digipin 7  
- Servo signaal → Arduino digipin 6  
- Mootoridraiveri IN1 → Arduino digipin 2  
- Mootoridraiveri IN2 → Arduino digipin 3  
- Mootoridraiveri ENA → Arduino pin 5 (PWM)  
- GND kõik ühises massis

👉 _Skeem peab lõpuks olemas olema, mitte ainult tekst._

---

## 7. Süsteemi juhtiv kood (või pseudokood)
**Kirjelda programmi loogikat nii, et seda on võimalik aru saada ka hiljem.**  
Kui kood töötab, pane siia lühike selgitus + viide failile `src/projektinimi.ino`.  
Kui kood pole veel valmis, lisa siia pseudokood.


dududu

# Mars Kolonisi Yaşam Destek Simülatörü

C diliyle yazılmış, metin tabanlı ve tek karakterli komutlarla kontrol edilen bir hayatta kalma simülasyonudur. Oyuncu, Mars'taki bir araştırma istasyonunda tek başına kalan bir mühendisi yönetir ve kurtarma ekibi gelene kadar **15 gün** hayatta kalmaya çalışır.

## Özellikler

- Üç zorluk seviyesi: Kolay, Normal, Zor
- Sekiz komut ve büyük/küçük harf duyarsız komut girişi
- Sağlık, enerji, oksijen, su, yedek parça ve modül hasarı takibi
- Rastgele olaylarda zar değerinin ve eşiğin ekrana yazdırılması
- Gün/tur sistemi ve oyun sonu puan raporu

## Kullanılan C Yapıları

| Yapı | Kullanıldığı yer |
|------|------------------|
| `switch-case` (fall-through) | Zorluk seçimi ve komut yönetimi |
| `if – else if – else` | Tamir kademeleri, keşif sonuçları, sınırlandırma |
| `for` + `break` + `continue` | Kum fırtınası (K) |
| `do-while` | Komut döngüsü, zorluk seçimi, hava kilidi şifresi (H) |
| `%` (mod) operatörü | Oksijen artığı, çift tur kontrolü, gün geçişi, şifre türetme |
| `&&`, `\|\|` | Yaralanma, su bulma eşiği, durum raporu |

## Derleme ve Çalıştırma

```bash
gcc mars_simulator.c -o mars_simulator
./mars_simulator
```

Windows için:

```bash
gcc mars_simulator.c -o mars_simulator.exe
mars_simulator.exe
```

## Komutlar

| Komut | İşlev |
|-------|-------|
| `G` | Keşif görevi: enerji −15, oksijen −10; su veya yedek parça bulunabilir |
| `O` | Oksijen üret: 2 su → 3 oksijen |
| `T` | Tamir: yedek parça harcayarak modül hasarını azaltır |
| `D` | Durum raporu (tur harcamaz) |
| `U` | Uyu / dinlen: enerji ve sağlık artar |
| `K` | Kum fırtınası simülasyonu |
| `H` | Hava kilidi şifresi (3 deneme) |
| `Q` | Çıkış |

Tanımsız komutlar uyarı verir ve tur harcamaz.

## Zorluk Seviyeleri

| Seviye | Başlangıç suyu | Yedek parça | Oksijen | Fırtına süresi | Fırtına taban hasarı |
|--------|----------------|-------------|---------|----------------|----------------------|
| Kolay (K) | 30 | 5 | 100 | 3 tur | 5 |
| Normal (N) | 20 | 3 | 100 | 5 tur | 8 |
| Zor (Z) | 10 | 2 | 90 | 7 tur | 12 |

## Oyun Kuralları

- Geçerli her komut bir tur harcar. Her 3 turda bir yeni gün başlar; gün başında oksijen 8 azalır, modül hasarı 70'in üzerindeyse ek olarak 5 sağlık kaybedilir.
- Sağlık veya oksijen 0'a düşerse oyun kaybedilir.
- 15. gün tamamlanırsa kurtarma ekibi gelir ve oyun kazanılır.
- Hava kilidi şifresi gün sayısından türetilir: `'A' + (gun % 26)`. Mühürlü kilit, fırtınadaki oksijen sızıntısını engeller.
- Puan: `(gun × 100) + saglik + oksijen − modulHasari`

## Örnek Ekran Çıktısı

```text
[Gun 1 | Tur 0] Saglik:100 Enerji:100 Oksijen:100 Su:20 Parca:3 Hasar:0 Kilit:ACIK
Komut> G
  Kesfe ciktin: enerji -15, oksijen -10.
  Zar: 76 / Esik: 10 -> GUVENLI
  Zar: 64 / Esik(su): 45, Esik(parca): 70
  Sonuc: YEDEK PARCA BULUNDU! +2 parca
```

## Dosya Yapısı

```text
.
├── mars_simulator.c   # Kaynak kod
└── README.md
```

## Not

Konsol kodlama sorunları yaşanmaması için ekran çıktılarında Türkçe karakter kullanılmamıştır.

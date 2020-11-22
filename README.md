# image-processing


Geometrik şekilleri farklı nesne algılama ve sınıflandırılması

Projede BMP uzantılı resim kullanılacak olup BMP dönüşüm işlemleri ders içerisinde verilen kaynak kodlarla yapılmıştır. Bahsi geçen fonksiyonlar ekte bulunan “kaynak.h” ve “kaynak.cpp” C++ çalışma sayfalarında bulunmaktadır.

# K-Means Clustering 

Giriş resminin BMP piksel değerleri kullanılarak kümeleme işlemi yapılması sağlanır. Algoritmanın adımları 5 adımda özetlenebilir;

  -1.	Küme merkezlerini rastgele ata
  -2.	Herbir örneğe kendisine en yakın kümenin etiketini ata
  -3.	Aynı etiketli örneklerin ortalamasını kullanarak yeni küme merkezlerini bul
  -4.	İterasyon sonlanana kadar 2-3 işlemlerini tekrarla
  -5. Bulunan küme merkezlerini döndür

“kaynak.h” dosyasında K-Means algoritması için 3 farklı fonksiyon bulunmaktadır.

# 1.Fonksiyon 
  - “D2_K_Mean_Clustering()C fonksiyonu sağlıklı bir Binary resim dönüşümü için kullanılmıştır. En ideal parlaklık değeri seçilerek Binary fonksiyonunda kullanılır.
# 2.Fonksiyon 
  - “MULTID_K_Means_Clustering()” fonksiyonu resmin N tane pikseli içerisinde k tane küme belirlenmesi için kullanılır ve bu kümeler için gri tonlama ile ayrım gösterilir.
# 3.Fonksiyon 
  - “MultD_K_Means_Clustering()” fonksiyonu resmin içerisinden kümeleri belirleyerek bunlara RGB değerleri vererek farklı renklerde gösterilmesi içindir.

# Binary Image

  “BinaryImage()” fonksiyonu aldığı parametreler arasında	"D2_K_Mean_Clustering()" fonksiyonundan gelen ideal piksel değerine göre resmi siyah-beyaz (0-255 parlaklık değeri) olarak ifade eder.
  
- BMP İmage
![image](https://github.com/tlhdrty/image-processing/blob/main/image.bmp)

- Binary İmage
![binary](https://github.com/tlhdrty/image-processing/blob/main/binary.bmp)

  
  
  

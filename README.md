# image-processing


Projede BMP uzantılı resim kullanılacak olup BMP dönüşüm işlemleri ders içerisinde verilen kaynak kodlarla yapılmıştır. Bahsi geçen fonksiyonlar ekte bulunan “kaynak.h” ve “kaynak.cpp” C++ çalışma sayfalarında bulunmaktadır.

# K-Means Clustering 

Giriş resminin BMP piksel değerleri kullanılarak kümeleme işlemi yapılması sağlanır. Algoritmanın adımları 5 adımda özetlenebilir;

  - 1.	Küme merkezlerini rastgele ata
  - 2.	Herbir örneğe kendisine en yakın kümenin etiketini ata
  - 3.	Aynı etiketli örneklerin ortalamasını kullanarak yeni küme merkezlerini bul
  - 4.	İterasyon sonlanana kadar 2-3 işlemlerini tekrarla
  - 5. Bulunan küme merkezlerini döndür

“kaynak.h” dosyasında K-Means algoritması için 3 farklı fonksiyon bulunmaktadır.

# 1.Fonksiyon 
  - “D2_K_Mean_Clustering()” C fonksiyonu sağlıklı bir Binary resim dönüşümü için kullanılmıştır. En ideal parlaklık değeri seçilerek Binary fonksiyonunda kullanılır.
# 2.Fonksiyon 
  - “MULTID_K_Means_Clustering()” fonksiyonu resmin N tane pikseli içerisinde k tane küme belirlenmesi için kullanılır ve bu kümeler için gri tonlama ile ayrım gösterilir.
# 3.Fonksiyon 
  - “MultD_K_Means_Clustering()” fonksiyonu resmin içerisinden kümeleri belirleyerek bunlara RGB değerleri vererek farklı renklerde gösterilmesi içindir.

# Binary Image

  “BinaryImage()” fonksiyonu aldığı parametreler arasında	"D2_K_Mean_Clustering()" fonksiyonundan gelen ideal piksel değerine göre resmi siyah-beyaz (0-255 parlaklık değeri) olarak ifade eder.
  
![image](https://github.com/tlhdrty/image-processing/blob/main/image.bmp?raw=true "BMP İmage")             ![binary](https://github.com/tlhdrty/image-processing/blob/main/binary.bmp?raw=true "Binary İmage")


# Dilation (Genişletme) 
  - Binary resme dönüşen resimdeki parazitleri gidermek için genişleme (or) işlemi yapılarak giderilir.

![dilation](https://github.com/tlhdrty/image-processing/blob/main/dilation.png?raw=true "Dilation (Genişletme)")

# Erosion (Aşındırma) 
  - Binary resimde birbirine yakın olan nesnelerin ayrılmasını sağlamak amacıyla kullanılmaktadır. İnceltme yapılarak diğer resimlere olan küçük bağlantıları kaldırmak için kullanılır.

![erosion](https://github.com/tlhdrty/image-processing/blob/main/erosion.png?raw=true "Erosion (Aşındırma)")

Daha verimli olması için dilation ve erosion arka arkaya uygulanır. Örnek verecek olursak pürüzleri gidermek için dilation işlemi yapıldıktan sonra nesnenin dış hattını geri getirmek için erosion işlemi uygulanır.

![two_prosess](https://github.com/tlhdrty/image-processing/blob/main/dilation-erosion_text.png?raw=true "example")

![process](https://github.com/tlhdrty/image-processing/blob/main/dilation-erosion.png?raw=true "example")

# RGB SEGMENT ve K-Means
  - “RGBSegment()” fonksiyonunda K-Means algoritmaında kullanılmak üzere R,G,B renk değerleri alabilecek bir dizi oluşturulur ve “MultD_K_Means_Clustering()” fonksiyonuna verilerek belirlenen kümeleri farklı renklerde ifade edilmesi sağlanır.
  
![image2](https://github.com/tlhdrty/image-processing/blob/main/image.bmp?raw=true "BMP İmage")             ![rgb](https://github.com/tlhdrty/image-processing/blob/main/RGBSegment.png?raw=true "RGB Segment")

# Boundary fonsiyonu 
  - Binary piksel değerlerine sahip bir resmin öncelikle Erosion işlemi uygulandıktan “Boundary()” fonksiyonu ile kenar çizgilerinin box ta tutularak oluşturulması sağlanır ve nesnelerin kenarları bu değerler ile ifade edilir. Tekrar resmi elde edbilmek için BMP dönüşümü gerekmektedir.
  

![image3](https://github.com/tlhdrty/image-processing/blob/main/image.bmp?raw=true "BMP İmage")             ![boundary](https://github.com/tlhdrty/image-processing/blob/main/boundary.png?raw=true "Boundary İmage")



  
  
  

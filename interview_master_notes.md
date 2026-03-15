# 🎯 Smart Maintenance Suite - Interview Master Notes

Bu belge, **Smart Maintenance Suite** projenizi bir iş görüşmesinde en profesyonel şekilde anlatabilmeniz ve projenin her detayına hakim olduğunuzu gösterebilmeniz için hazırlanmıştır.

---

## 🚀 1. Proje Özeti (Elevator Pitch)
**Smart Maintenance Suite**, endüstriyel tesislerdeki makinelerin durumunu gerçek zamanlı izleyen, kestirimci bakım (predictive maintenance) planlayan ve duruş sürelerini minimize eden bir **Endüstriyel IoT (IIoT)** yönetim platformudur.
*   **Backend:** Yüksek performans ve düşük gecikme için **C** dili ile geliştirildi.
*   **Frontend:** Dinamik ve kullanıcı dostu bir arayüz için **React** kullanıldı.
*   **Veri Yönetimi:** **PostgreSQL** veritabanı, özel bir bağlantı havuzu (connection pool) ile optimize edildi.
*   **Altyapı:** **Docker** konteynerizasyonu, **GitHub Actions** CI/CD süreçleri ve **Doxygen** tabanlı teknik dokümantasyon.

---

## 🏗️ 2. Teknik Derinlik (Backend - C)

### A. Neden C Dili?
*   **Performans:** Gerçek zamanlı sensör verilerinin (sıcaklık, titreşim vb.) milisaniyeler seviyesinde işlenmesi gerekir. C, donanıma yakınlığıyla bu hızı sağlar.
*   **IoT Uyumluluğu:** IIoT cihazları genellikle kısıtlı kaynaklara sahiptir. C dili, bellek ve CPU yönetimini manuel yapmamıza olanak tanıyarak verimliliği artırır.

### B. Veri Yapıları ve Kullanım Amaçları
Mülakatta "Neden bu veri yapılarını kullandın?" sorusuna hazır olun:
*   **Heap (Yığın):** Bakım görevleri için bir **Priority Queue (Öncelikli Kuyruk)** oluşturmakta kullanıldı. Kritik durumdaki makineler (yüksek sıcaklık uyarısı verenler) listenin en başına alınır.
*   **BST (İkili Arama Ağacı):** On binlerce makine veya yedek parça arasında ID bazlı hızlı arama (O(log n)) yapmak için.
*   **Graph (Çizge):** Fabrika yerleşimi veya makine bağımlılıklarını modellemek için. Bir makine durduğunda hangi üretim hattının etkileneceğini tespit eder.
*   **Queue/Stack (Kuyruk/Yığın):** Sensör verilerinin sıra ile işlenmesi (FIFO) ve işlem geçmişinin (Undo/Redo veya Transaction Log) tutulması (LIFO) için.

### C. Veri Yönetimi ve Performans (Database Connection Pooling)
*   **Bağlantı Havuzu (Connection Pool):** Her veritabanı isteği için yeni bir TCP bağlantısı açmak maliyetlidir. Projede, uygulama başlangıcında belirli sayıda bağlantıyı hazırda tutan ve istek bittiğinde bunları havuza geri bırakan (`db_pool_acquire` / `db_pool_release`) özel bir yapı kuruldu. Bu, uygulamanın ölçeklenebilirliğini (scalability) artırır.
*   **PostgreSQL Entegrasyonu:** `libpq` kütüphanesi kullanılarak ham SQL sorguları ile veritabanı yönetildi. Bu, ORM'lerin getirdiği overhead'den kaçınarak maksimum hıza ulaşmamızı sağladı.

### D. Güvenerlik ve Yetkilendirme (Security)
*   **JWT (JSON Web Token):** C dilinde standart bir JWT kütüphanesi hazır gelmez. Bu projede **özel bir JWT üretici ve doğrulayıcı** yazıldı (HMAC-SHA256).
*   **RBAC (Rol Bazlı Erişim Kontrolü):** Admin (Tam yetki), Technician (Bakım girişi), Operator (Gözlemci) rolleri veri katmanında sıkı bir şekilde kontrol edilir.

### E. Test Stratejisi (Mocking & Unit Testing)
*   Veritabanına bağımlı kalmadan iş mantığını (business logic) test edebilmek için `#ifdef TEST_MODE` makroları kullanıldı.
*   Gerçek veritabanı yerine **Mock** (taklit) verilerle çalışan fonksiyonlar yazılarak CI/CD süreçlerinde testlerin milisaniyeler içinde tamamlanması sağlandı.

---

## 💻 3. Frontend Mimarisi (React)

*   **State Management:** Verilerin bileşenler arasında temiz bir şekilde paylaşılması için **React Context API** kullanıldı.
*   **Veri Görselleştirme:** Sensör verileri anlık grafiklere (Analytics Panel) döküldü.
*   **Responsive Design:** Fabrika ortamında tablet veya telefon üzerinden de kullanılabilmesi için CSS Media Queries ile esnek tasarım sağlandı.

---

## 🛠️ 4. Mühendislik Standartları ve Kalite (The "Wow" Factors)

Bir mülakatta sizi diğerlerinden ayıracak özellikler:
1.  **Yüksek Test Kapsamı (Test Coverage):** Backend'de %90, Frontend'de %96 koruma sağlandı. Sadece kod yazmakla kalmayıp, kodun sağlamlığını da garanti altına aldınız.
2.  **Bellek Yönetimi:** C kısmında `valgrind` ve özel log mekanizmalarıyla bellek sızıntıları (memory leaks) sıfıra indirildi.
3.  **Cross-Platform Destek:** Projenin hem **Windows** hem de **Linux (WSL)** üzerinde sorunsuz derlenip çalışması için kompleks CMake konfigürasyonları ve otomasyon scriptleri yazıldı.
4.  **Dokümantasyon:** Kodun her satırı doxygen standartlarında dökümante edildi, teknik tasarım şemaları (UML) PlantUML ile otomatik oluşturuldu.

---

## ❓ 5. Olası Mülakat Soruları ve Örnek Cevaplar

**S1: C backend ile React frontend'i nasıl haberleştirdin?**
*   **Cevap:** Backend tarafında **Mongoose** (veya benzeri bir HTTP modülü) kullanarak bir REST API sunucusu ayağa kaldırdım. Verileri **JSON (cJSON kütüphanesi)** formatında seriye bağlayıp (serialization), React'in `fetch` veya `axios` ile bu endpoint'leri tüketmesini sağladım.

**S2: Projede karşılaştığın en büyük teknik zorluk neydi?**
*   **Cevap:** C dilinde bellek güvenliğini (memory safety) sağlamak ve veritabanı bağlantılarını yönetmekti. Özellikle yüksek yük altında veritabanının tıkanmaması için bir **Bağlantı Havuzu (Connection Pool)** tasarımı uyguladım. Bu sayede her istekte yeni bağlantı açmak yerine var olanları verimli kullandım.

**S3: Neden PostgreSQL tercih ettin?**
*   **Cevap:** Endüstriyel veriler yapılandırılmıştır. ACID prensiplerine tam uyum, karmaşık sorgulardaki (JOIN işlemleri) performansı ve JSONB desteği sayesinde PostgreSQL, IIoT verileri için en güvenilir tercihti.

**S4: Docker neden gerekliydi?**
*   **Cevap:** "Benim makinemde çalışıyor" sorununu çözmek için. Backend (C derleyicisi, libpq) ve Frontend (Node.js) bağımlılıklarını izole ederek, projenin herhangi bir sunucuda tek komutla (`docker-compose up`) ayağa kalkmasını sağladım.

---

## 📈 6. Proje İstatistikleri (Akılda Kalıcı Rakamlar)
*   **100+** Unit Test
*   **%90+** Kod Kapsamı (Coverage)
*   **3** Farklı Kullanıcı Rolü
*   **5** Temel Veri Yapısı (Implementasyonu size ait)
*   **Full Dockerized** Dağıtım

---

### 💡 Son Tavsiye
Mülakatta ekran paylaşımı istenirse, `10-generate_backend_coverage.bat` gibi scriptlerinizi ve oluşturduğunuz profesyonel HTML raporlarını gösterin. Bir yazılımcı için "çalışan kod" kadar, o kodu nasıl "test ettiği ve dökümante ettiği" de önemlidir.

**Başarılar dilerim!**

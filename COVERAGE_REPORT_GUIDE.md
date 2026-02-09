# 📊 Professional Coverage Report Guide

## Görseldeki Gibi Birleşik Coverage Raporu Nasıl Oluşturulur?

### 🎯 Hedef
Tüm modüllerin (Backend, Frontend, Data Structures, Security, Database, API) coverage'ını **tek bir profesyonel HTML raporunda** görmek.

---

## 🚀 Hızlı Başlangıç

### Yöntem 1: Professional Report (Önerilen - Görseldeki Gibi)

```bash
# Önce testleri derle
run-backend-tests.bat

# Sonra profesyonel rapor oluştur
10-professional-coverage-report.bat
```

Bu komut:
- ✅ Tüm backend testlerini OpenCppCoverage ile çalıştırır
- ✅ Frontend testlerini Vitest coverage ile çalıştırır
- ✅ ReportGenerator ile birleşik HTML rapor oluşturur
- ✅ Coverage history tutar (trend grafiği)
- ✅ Badges oluşturur
- ✅ Otomatik tarayıcıda açar

### Yöntem 2: Unified Report (gcov + ReportGenerator)

```bash
9-unified-test-coverage.bat
```

Bu komut:
- ✅ Backend testlerini gcov coverage ile derler ve çalıştırır
- ✅ Frontend testlerini çalıştırır
- ✅ gcovr ile XML formatına çevirir
- ✅ ReportGenerator ile birleştirir

---

## 📦 Gerekli Araçlar

### 1. OpenCppCoverage (Windows - Önerilen)
```bash
choco install opencppcoverage -y
```

**Veya** MinGW GCC (gcov ile):
```bash
# Zaten yüklü olmalı
gcc --version
```

### 2. ReportGenerator (.NET Global Tool)
```bash
dotnet tool install -g dotnet-reportgenerator-globaltool
```

### 3. Python + gcovr (Fallback için)
```bash
pip install gcovr
```

### 4. Node.js + npm
```bash
# Frontend testleri için
node --version
npm --version
```

---

## 📂 Oluşturulan Raporlar

### Professional Report (`10-professional-coverage-report.bat`)

```
coverage_professional/
├── combined/
│   ├── index.html              ← 🌐 ANA RAPOR (Görseldeki gibi)
│   ├── badge_linecoverage.svg  ← Coverage badge
│   ├── summary.txt             ← Metin özeti
│   └── ...
├── backend/
│   └── html/
│       └── index.html          ← Backend-only rapor
├── frontend/
│   └── index.html              ← Frontend-only rapor
└── history/
    └── ...                     ← Coverage trend grafiği için
```

### Unified Report (`9-unified-test-coverage.bat`)

```
coverage_unified/
├── combined/
│   └── index.html              ← Birleşik rapor
├── backend/
│   ├── coverage.xml
│   └── html/
└── frontend/
    └── lcov.info
```

---

## 🎨 Rapor Özellikleri

### Combined Report İçeriği

1. **Summary Dashboard** (Görseldeki gibi)
   - Line coverage %
   - Branch coverage %
   - Method coverage %
   - Coverage history graph

2. **Module Breakdown**
   - Data Structures (Queue, Stack, Heap, BST, Graph)
   - Security (RBAC, JWT)
   - Database (Machine, Inventory, Maintenance)
   - API (Handlers, Router)
   - Frontend (Components, Context)

3. **File-Level Details**
   - Her dosya için detaylı coverage
   - Satır satır hangi kodun test edildiği
   - Kırmızı/yeşil renk kodlaması

4. **Coverage History**
   - Zaman içinde coverage değişimi
   - Trend grafiği
   - Build-to-build karşılaştırma

---

## 📊 Coverage Targets

| Module | Target | Current |
|--------|--------|---------|
| Data Structures | >90% | ✅ |
| Security | >85% | ✅ |
| Database | >75% | ⏳ |
| API | >80% | ⏳ |
| Frontend | >80% | ⏳ |
| **Overall** | **>80%** | **⏳** |

---

## 🔄 Workflow

### Her Test Sonrası

```bash
# 1. Testleri çalıştır
run-backend-tests.bat

# 2. Coverage raporu oluştur
10-professional-coverage-report.bat

# 3. Raporu incele
# Otomatik tarayıcıda açılır: coverage_professional/combined/index.html
```

### CI/CD Pipeline İçin

```yaml
# GitHub Actions örneği
- name: Run Tests with Coverage
  run: |
    run-backend-tests.bat
    10-professional-coverage-report.bat

- name: Upload Coverage Report
  uses: actions/upload-artifact@v3
  with:
    name: coverage-report
    path: coverage_professional/combined/
```

---

## 🐛 Sorun Giderme

### OpenCppCoverage bulunamadı
```bash
# Chocolatey ile yükle
choco install opencppcoverage -y

# PATH'e eklendiğini kontrol et
where OpenCppCoverage
```

### ReportGenerator bulunamadı
```bash
# .NET SDK yükle (önce)
winget install Microsoft.DotNet.SDK.8

# ReportGenerator yükle
dotnet tool install -g dotnet-reportgenerator-globaltool

# PATH'e eklendiğini kontrol et
where reportgenerator
```

### gcovr bulunamadı
```bash
# Python ile yükle
pip install gcovr

# Kontrol et
python -m gcovr --version
```

### Frontend testleri çalışmıyor
```bash
cd src/frontend
npm install
npm test
```

---

## 📈 Coverage Artırma İpuçları

### 1. Eksik Test Alanlarını Bul
Raporda kırmızı satırlar = test edilmemiş kod

### 2. Edge Case'leri Test Et
- NULL inputs
- Boundary conditions
- Error paths

### 3. Integration Tests Ekle
- Database bağlantılı testler
- API endpoint testleri
- E2E testler

---

## 🎯 Sonraki Adımlar

1. ✅ `run-backend-tests.bat` çalıştır
2. ✅ `10-professional-coverage-report.bat` çalıştır
3. 📊 Raporu incele: `coverage_professional/combined/index.html`
4. 🔍 Kırmızı alanları tespit et
5. ✍️ Eksik testleri yaz
6. 🔄 Tekrar çalıştır ve coverage'ı artır

---

## 📸 Örnek Rapor Görünümü

Oluşturulan rapor görseldeki gibi olacak:

- **Summary Panel**: Line/Branch/Method coverage yüzdeleri
- **Coverage History Graph**: Zaman içinde değişim
- **Module List**: Her modülün coverage'ı
- **File Details**: Satır satır coverage
- **Color Coding**: Yeşil (covered), Kırmızı (not covered)

---

**Not**: İlk çalıştırmada tüm araçları yüklemesi 5-10 dakika sürebilir. Sonraki çalıştırmalar çok daha hızlı olacak.

import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  // 1. Ana esbuild derleyicisine .js dosyalarını 'jsx' olarak oku diyoruz
  esbuild: {
    loader: 'jsx',
    include: /src\/.*\.js$/,
    exclude: [],
  },
  // 2. Başlangıçtaki bağımlılık tarayıcısına (optimizasyon) aynı talimatı veriyoruz
  optimizeDeps: {
    esbuildOptions: {
      loader: {
        '.js': 'jsx',
      },
    },
  },
})

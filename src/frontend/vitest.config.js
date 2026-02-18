import { defineConfig } from 'vitest/config'
import react from '@vitejs/plugin-react'
import path from 'path'

export default defineConfig({
  plugins: [react()],

  esbuild: {
    loader: 'jsx',
    include: /src\/.*\.js$|tests\/.*\.js$/,
    exclude: [],
  },

  server: {
    fs: {
      allow: [
        path.resolve(__dirname, '../../'),
      ]
    }
  },

  test: {
    globals: true,
    environment: 'jsdom',
    include: [
      '../tests/frontend/unit/**/*.{test,spec}.{js,jsx}'
    ],
    setupFiles: './test-setup.js',

    coverage: {
      provider: 'v8',
      reporter: ['text', 'json', 'html', 'lcov'],
      // Dışarıdaki .bat dosyasının lcov.info'yu bulabilmesi için dizini netleştiriyoruz
      reportsDirectory: path.resolve(__dirname, 'coverage'),
      all: true,
      include: ['src/**/*.{js,jsx}'],
      exclude: [
        'node_modules/',
        'src/main.js',
        '**/*.test.{js,jsx}',
        '**/*.config.{js,jsx}'
      ]
    }
  },

  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@testing-library/react': path.resolve(__dirname, 'node_modules/@testing-library/react'),
      '@testing-library/jest-dom': path.resolve(__dirname, 'node_modules/@testing-library/jest-dom'),
      '@testing-library/user-event': path.resolve(__dirname, 'node_modules/@testing-library/user-event'),
      'vitest': path.resolve(__dirname, 'node_modules/vitest')
    }
  }
})
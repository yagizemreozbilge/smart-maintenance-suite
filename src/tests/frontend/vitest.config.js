import { defineConfig } from 'vitest/config';
import react from '@vitejs/plugin-react';
import path from 'path';

export default defineConfig({
    plugins: [react()],
    test: {
        globals: true,
        environment: 'jsdom',
        include: [
      '../tests/frontend/unit/**/*.{test,spec}.{js,jsx}'
    ],
        setupFiles: './setup.js',
        coverage: {
            provider: 'v8',
            reporter: ['text', 'json', 'html', 'lcov'],
            reportsDirectory: './coverage',
            include: ['../../frontend/src/**/*.{js,jsx}'],
            exclude: [
                'node_modules/',
                '../../frontend/src/main.js',
                '**/*.test.{js,jsx}',
                '**/*.config.{js,jsx}'
            ]
        }
    },
    resolve: {
        alias: {
            '@': path.resolve(__dirname, '../../frontend/src')
        }
    }
});

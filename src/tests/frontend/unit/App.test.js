import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen, waitFor } from '@testing-library/react';
import '@testing-library/jest-dom';
import App from '../../../frontend/src/App';

// Mock localStorage
const localStorageMock = {
    getItem: vi.fn(),
    setItem: vi.fn(),
    clear: vi.fn(),
    removeItem: vi.fn()
};
global.localStorage = localStorageMock;

// Mock fetch
global.fetch = vi.fn();

describe('App Component', () => {
    beforeEach(() => {
        vi.clearAllMocks();
        localStorageMock.getItem.mockReturnValue(null);
    });

    it('should render login page when not authenticated', () => {
        render(<App />);
        expect(screen.getByText(/Smart HMS/i)).toBeInTheDocument();
    });

    it('should check localStorage for auth token on mount', () => {
        render(<App />);
        expect(localStorageMock.getItem).toHaveBeenCalledWith('auth_token');
    });

    it('should render dashboard when authenticated', () => {
        localStorageMock.getItem.mockReturnValue('mock_token_123');
        render(<App />);

        // Dashboard should be visible
        expect(screen.queryByText(/Smart HMS/i)).not.toBeInTheDocument();
    });

    it('should have logout button when authenticated', () => {
        localStorageMock.getItem.mockReturnValue('mock_token_123');
        render(<App />);

        const logoutButton = screen.getByText(/SİSTEMDEN ÇIKIŞ YAP/i);
        expect(logoutButton).toBeInTheDocument();
    });
});

describe('App Authentication Flow', () => {
    it('should handle login state change', async () => {
        const { rerender } = render(<App />);

        // Initially not authenticated
        expect(screen.getByText(/Smart HMS/i)).toBeInTheDocument();

        // Simulate login
        localStorageMock.getItem.mockReturnValue('new_token');
        rerender(<App />);

        await waitFor(() => {
            expect(screen.queryByText(/Smart HMS/i)).not.toBeInTheDocument();
        });
    });

    it('should clear localStorage on logout', () => {
        localStorageMock.getItem.mockReturnValue('mock_token');
        const { getByText } = render(<App />);

        const logoutBtn = getByText(/SİSTEMDEN ÇIKIŞ YAP/i);
        logoutBtn.click();

        expect(localStorageMock.clear).toHaveBeenCalled();
    });
});

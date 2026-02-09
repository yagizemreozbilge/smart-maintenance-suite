import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import '@testing-library/jest-dom';
import LoginPage from '../../../frontend/src/components/LoginPage';

global.fetch = vi.fn();

describe('LoginPage Component', () => {
    const mockOnLogin = vi.fn();

    beforeEach(() => {
        vi.clearAllMocks();
    });

    it('should render login form', () => {
        render(<LoginPage onLogin={mockOnLogin} />);

        expect(screen.getByText(/Smart HMS/i)).toBeInTheDocument();
        expect(screen.getByPlaceholderText(/admin/i)).toBeInTheDocument();
        expect(screen.getByPlaceholderText(/••••••••/i)).toBeInTheDocument();
    });

    it('should have username and password inputs', () => {
        render(<LoginPage onLogin={mockOnLogin} />);

        const usernameInput = screen.getByPlaceholderText(/admin/i);
        const passwordInput = screen.getByPlaceholderText(/••••••••/i);

        expect(usernameInput).toBeInTheDocument();
        expect(passwordInput).toBeInTheDocument();
        expect(passwordInput.type).toBe('password');
    });

    it('should update input values on change', () => {
        render(<LoginPage onLogin={mockOnLogin} />);

        const usernameInput = screen.getByPlaceholderText(/admin/i);
        const passwordInput = screen.getByPlaceholderText(/••••••••/i);

        fireEvent.change(usernameInput, { target: { value: 'testuser' } });
        fireEvent.change(passwordInput, { target: { value: 'testpass' } });

        expect(usernameInput.value).toBe('testuser');
        expect(passwordInput.value).toBe('testpass');
    });

    it('should call API on form submit', async () => {
        fetch.mockResolvedValueOnce({
            json: async () => ({ success: true, token: 'mock_token', role: 'admin', username: 'admin' })
        });

        render(<LoginPage onLogin={mockOnLogin} />);

        const usernameInput = screen.getByPlaceholderText(/admin/i);
        const passwordInput = screen.getByPlaceholderText(/••••••••/i);
        const submitButton = screen.getByText(/SİSTEME GİRİŞ YAP/i);

        fireEvent.change(usernameInput, { target: { value: 'admin' } });
        fireEvent.change(passwordInput, { target: { value: 'admin123' } });
        fireEvent.click(submitButton);

        await waitFor(() => {
            expect(fetch).toHaveBeenCalledWith(
                expect.stringContaining('/api/login?u=admin&p=admin123')
            );
        });
    });

    it('should show error on failed login', async () => {
        fetch.mockResolvedValueOnce({
            json: async () => ({ success: false, message: 'Invalid credentials' })
        });

        render(<LoginPage onLogin={mockOnLogin} />);

        const usernameInput = screen.getByPlaceholderText(/admin/i);
        const passwordInput = screen.getByPlaceholderText(/••••••••/i);
        const submitButton = screen.getByText(/SİSTEME GİRİŞ YAP/i);

        fireEvent.change(usernameInput, { target: { value: 'wrong' } });
        fireEvent.change(passwordInput, { target: { value: 'wrong' } });
        fireEvent.click(submitButton);

        await waitFor(() => {
            expect(screen.getByText(/Hatalı kullanıcı adı veya şifre/i)).toBeInTheDocument();
        });
    });

    it('should show loading state during login', async () => {
        fetch.mockImplementationOnce(() => new Promise(resolve => setTimeout(resolve, 100)));

        render(<LoginPage onLogin={mockOnLogin} />);

        const submitButton = screen.getByText(/SİSTEME GİRİŞ YAP/i);
        fireEvent.click(submitButton);

        expect(screen.getByText(/SİSTEME GİRİLİYOR/i)).toBeInTheDocument();
    });

    it('should store token on successful login', async () => {
        const mockLocalStorage = {
            setItem: vi.fn()
        };
        global.localStorage = mockLocalStorage;

        fetch.mockResolvedValueOnce({
            json: async () => ({
                success: true,
                token: 'test_token_123',
                role: 'admin',
                username: 'admin'
            })
        });

        render(<LoginPage onLogin={mockOnLogin} />);

        const usernameInput = screen.getByPlaceholderText(/admin/i);
        const passwordInput = screen.getByPlaceholderText(/••••••••/i);
        const submitButton = screen.getByText(/SİSTEME GİRİŞ YAP/i);

        fireEvent.change(usernameInput, { target: { value: 'admin' } });
        fireEvent.change(passwordInput, { target: { value: 'admin123' } });
        fireEvent.click(submitButton);

        await waitFor(() => {
            expect(mockLocalStorage.setItem).toHaveBeenCalledWith('auth_token', 'test_token_123');
            expect(mockLocalStorage.setItem).toHaveBeenCalledWith('user_role', 'admin');
            expect(mockLocalStorage.setItem).toHaveBeenCalledWith('username', 'admin');
        });
    });
});

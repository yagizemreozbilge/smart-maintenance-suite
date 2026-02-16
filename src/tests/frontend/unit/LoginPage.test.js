import { render, screen, fireEvent, waitFor } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import LoginPage from '../../frontend/src/components/LoginPage'

describe('LoginPage', () => {

  beforeEach(() => {
    vi.clearAllMocks()
    localStorage.clear()
  })

  it('Input değişimi çalışıyor', () => {
    render(<LoginPage onLogin={() => {}} />)

    const usernameInput = screen.getByPlaceholderText('admin')
    fireEvent.change(usernameInput, { target: { value: 'testUser' } })

    expect(usernameInput.value).toBe('testUser')
  })

  it('Başarılı login', async () => {
    global.fetch = vi.fn(() =>
      Promise.resolve({
        json: () => Promise.resolve({
          success: true,
          token: 'abc',
          role: 'admin',
          username: 'test'
        })
      })
    )

    const onLogin = vi.fn()

    render(<LoginPage onLogin={onLogin} />)

    fireEvent.change(screen.getByPlaceholderText('admin'), {
      target: { value: 'admin' }
    })

    fireEvent.change(screen.getByPlaceholderText('••••••••'), {
      target: { value: '1234' }
    })

    fireEvent.click(screen.getByText(/SİSTEME GİRİŞ YAP/i))

    await waitFor(() => {
      expect(onLogin).toHaveBeenCalled()
    })

    expect(localStorage.getItem('auth_token')).toBe('abc')
  })

  it('Hatalı login error gösterir', async () => {
    global.fetch = vi.fn(() =>
      Promise.resolve({
        json: () => Promise.resolve({ success: false })
      })
    )

    render(<LoginPage onLogin={() => {}} />)

    fireEvent.click(screen.getByText(/SİSTEME GİRİŞ YAP/i))

    await waitFor(() => {
      expect(screen.getByText(/Hatalı kullanıcı adı/i)).toBeInTheDocument()
    })
  })
})
import { render, screen, fireEvent, waitFor } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import App from '../../frontend/src/App'

// fetch mock (Dashboard sync için)
global.fetch = vi.fn(() =>
  Promise.resolve({
    json: () => Promise.resolve([])
  })
)

describe('App Component', () => {
  beforeEach(() => {
    localStorage.clear()
    vi.clearAllMocks()
  })

  it('Login yoksa LoginPage render edilir', () => {
    render(<App />)
    expect(screen.getByText(/Smart HMS/i)).toBeInTheDocument()
  })

  it('Token varsa dashboard render edilir', async () => {
    localStorage.setItem('auth_token', 'test-token')

    render(<App />)

    await waitFor(() => {
      expect(screen.getByText(/SİSTEMDEN ÇIKIŞ YAP/i)).toBeInTheDocument()
    })
  })

  it('Logout çalışır', async () => {
    localStorage.setItem('auth_token', 'test-token')

    render(<App />)

    const logoutBtn = await screen.findByText(/SİSTEMDEN ÇIKIŞ YAP/i)
    fireEvent.click(logoutBtn)

    expect(screen.getByText(/Smart HMS/i)).toBeInTheDocument()
  })
})
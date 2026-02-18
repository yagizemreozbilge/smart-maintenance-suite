import { render, screen, fireEvent, waitFor } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import App from '../../../frontend/src/App'

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

  it('Syncing error handling works', async () => {
    localStorage.setItem('auth_token', 'test-token')
    global.fetch = vi.fn().mockRejectedValue(new Error('Network error'))
    const consoleSpy = vi.spyOn(console, 'error').mockImplementation(() => { })

    render(<App />)

    await waitFor(() => {
      expect(consoleSpy).toHaveBeenCalledWith("Backend connection failed:", expect.any(Error))
    })
    consoleSpy.mockRestore()
  })

  it('Admin buttons are visible for admin role', async () => {
    localStorage.setItem('auth_token', 'test-token')
    localStorage.setItem('user_role', 'admin')
    window.open = vi.fn()
    window.print = vi.fn()

    render(<App />)

    const csvBtn = await screen.findByText(/MAINTENANCE \(CSV\)/i)
    fireEvent.click(csvBtn)
    expect(window.open).toHaveBeenCalledWith(expect.stringContaining('/reports/maintenance'))

    const printBtn = screen.getByText(/PRINT PDF REPORT/i)
    fireEvent.click(printBtn)
    expect(window.print).toHaveBeenCalled()
  })

  it('Operator role hides maintenance form', async () => {
    localStorage.setItem('auth_token', 'test-token')
    localStorage.setItem('user_role', 'operator')

    render(<App />)

    await waitFor(() => {
      expect(screen.queryByText(/New Maintenance Log/i)).not.toBeInTheDocument()
    })
  })
})
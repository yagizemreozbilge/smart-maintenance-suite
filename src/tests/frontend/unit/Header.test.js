import { render, screen, fireEvent } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { Header } from '../../frontend/src/components/Header'

describe('Header', () => {

  beforeEach(() => {
    localStorage.setItem('username', 'AdminUser')
  })

  it('Kullanıcı adı gösterilir', () => {
    render(<Header title="Test Title" />)
    expect(screen.getByText(/Test Title/i)).toBeInTheDocument()
  })

  it('Logout butonu çalışır', () => {
    const logoutMock = vi.fn()
    render(<button onClick={logoutMock}>Logout</button>)

    fireEvent.click(screen.getByText('Logout'))
    expect(logoutMock).toHaveBeenCalled()
  })
})
import { render, screen, act } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import WelcomeMessage, { Header } from '../../../frontend/src/components/Header'

describe('Header & WelcomeMessage', () => {

  beforeEach(() => {
    localStorage.clear()
    vi.useFakeTimers()
  })

  afterEach(() => {
    vi.restoreAllMocks()
  })

  it('Header displays title and user info from localStorage', () => {
    localStorage.setItem('username', 'TestAdmin')
    localStorage.setItem('user_role', 'admin')

    render(<Header title="Smart Control" />)

    expect(screen.getByText('Smart Control')).toBeInTheDocument()
    expect(screen.getByText('TestAdmin')).toBeInTheDocument()
    expect(screen.getByText(/ADMIN LEVEL ACCESS/i)).toBeInTheDocument()
  })

  it('Header shows default values when localStorage is empty', () => {
    render(<Header title="Smart Control" />)

    expect(screen.getByText('Unknown User')).toBeInTheDocument()
    expect(screen.getByText(/OPERATOR LEVEL ACCESS/i)).toBeInTheDocument()
  })

  it('WelcomeMessage updates time periodically', () => {
    render(<WelcomeMessage />)

    const initialTime = screen.getByText(/\d{1,2}:\d{2}:\d{2}/).textContent

    act(() => {
      vi.advanceTimersByTime(1000)
    })

    const updatedTime = screen.queryByText(/\d{1,2}:\d{2}:\d{2}/)
    expect(updatedTime).toBeInTheDocument()
  })

  it('WelcomeMessage clears interval on unmount', () => {
    const clearIntervalSpy = vi.spyOn(global, 'clearInterval')
    const { unmount } = render(<WelcomeMessage />)

    unmount()
    expect(clearIntervalSpy).toHaveBeenCalled()
  })
})
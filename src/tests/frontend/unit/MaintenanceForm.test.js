import { render, screen, fireEvent } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { MaintenanceForm } from '../../frontend/src/components/MaintenanceForm'
import { MaintenanceProvider } from '../../frontend/src/context/MaintenanceContext'

// dispatch mocklamak için context override
vi.mock('../../frontend/src/context/MaintenanceContext', async (orig) => {
  const actual = await orig()
  return {
    ...actual,
    useMaintenanceDispatch: () => vi.fn()
  }
})

describe('MaintenanceForm', () => {

  it('Form render edilir', () => {
    render(<MaintenanceProvider><MaintenanceForm /></MaintenanceProvider>)
    expect(screen.getByText(/Equipment Onboarding/i)).toBeInTheDocument()
  })

  it('Input change çalışır', () => {
    render(<MaintenanceProvider><MaintenanceForm /></MaintenanceProvider>)
    const input = screen.getByPlaceholderText(/Laser Cutter/i)
    fireEvent.change(input, { target: { value: 'Test Machine' } })
    expect(input.value).toBe('Test Machine')
  })

  it('Boş submit dispatch çağırmaz', () => {
    const dispatchMock = vi.fn()

    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenanceDispatch')
      .mockReturnValue(dispatchMock)

    render(<MaintenanceForm />)

    fireEvent.click(screen.getByText(/Register Asset/i))

    expect(dispatchMock).not.toHaveBeenCalled()
  })

  it('Doğru submit dispatch çağırır', () => {
    const dispatchMock = vi.fn()

    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenanceDispatch')
      .mockReturnValue(dispatchMock)

    render(<MaintenanceForm />)

    fireEvent.change(screen.getByPlaceholderText(/Laser Cutter/i), {
      target: { value: 'Machine X' }
    })

    fireEvent.click(screen.getByText(/Register Asset/i))

    expect(dispatchMock).toHaveBeenCalled()
  })
})
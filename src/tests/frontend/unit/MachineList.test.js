import { render, screen, fireEvent } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { MachineList } from '../../frontend/src/components/MachineList'
import { MaintenanceProvider } from '../../frontend/src/context/MaintenanceContext'

// Context mock
vi.mock('../../frontend/src/context/MaintenanceContext', async (orig) => {
  const actual = await orig()
  return {
    ...actual,
    useMaintenance: () => ({
      machines: []
    }),
    useMaintenanceDispatch: () => vi.fn()
  }
})

describe('MachineList', () => {

  it('Boş liste render edilir (no machine)', () => {
    render(<MaintenanceProvider><MachineList /></MaintenanceProvider>)
    expect(screen.queryByRole('heading')).not.toBeNull()
  })

  it('Liste doluysa machine render edilir', () => {
    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenance')
      .mockReturnValue({
        machines: [
          { id: 1, name: 'Pump', location: 'A1', status: 'operational' }
        ]
      })

    render(<MachineList />)

    expect(screen.getByText('Pump')).toBeInTheDocument()
  })

  it('Search filtre çalışır', () => {
    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenance')
      .mockReturnValue({
        machines: [
          { id: 1, name: 'Pump', location: 'A1', status: 'operational' },
          { id: 2, name: 'Motor', location: 'A2', status: 'operational' }
        ]
      })

    render(<MachineList />)

    fireEvent.change(screen.getByPlaceholderText(/Search assets/i), {
      target: { value: 'Pump' }
    })

    expect(screen.getByText('Pump')).toBeInTheDocument()
  })
})
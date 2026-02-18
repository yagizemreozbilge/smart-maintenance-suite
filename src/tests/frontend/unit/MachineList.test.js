import { render, screen, fireEvent, waitFor } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import { MachineList } from '../../../frontend/src/components/MachineList'
import { MaintenanceProvider } from '../../../frontend/src/context/MaintenanceContext'
import * as MaintenanceContext from '../../../frontend/src/context/MaintenanceContext'

// Context mock
vi.mock('../../../frontend/src/context/MaintenanceContext', async (orig) => {
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

  beforeEach(() => {
    localStorage.clear()
    vi.clearAllMocks()
  })

  it('Boş liste render edilir (no machine)', () => {
    render(<MaintenanceProvider><MachineList /></MaintenanceProvider>)
    expect(screen.queryByRole('heading', { name: /Assets Inventory/i })).toBeInTheDocument()
  })

  it('Liste doluysa machine render edilir', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        machines: [
          { id: 1, name: 'Pump', location: 'A1', status: 'Operational' }
        ]
      })

    render(<MachineList />)

    expect(screen.getByText('Pump')).toBeInTheDocument()
  })

  it('Search filtre çalışır', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        machines: [
          { id: 1, name: 'Pump', location: 'A1', status: 'Operational' },
          { id: 2, name: 'Motor', location: 'A2', status: 'Operational' }
        ]
      })

    render(<MachineList />)

    fireEvent.change(screen.getByPlaceholderText(/Search assets/i), {
      target: { value: 'Pump' }
    })

    expect(screen.getByText('Pump')).toBeInTheDocument()
    expect(screen.queryByText('Motor')).not.toBeInTheDocument()
  })

  it('Status filtreleme çalışır', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        machines: [
          { id: 1, name: 'Pump', location: 'A1', status: 'Operational' },
          { id: 2, name: 'Warning Pump', location: 'A2', status: 'Warning' }
        ]
      })

    render(<MachineList />)

    fireEvent.click(screen.getByRole('button', { name: 'Warning' }))
    expect(screen.getByText('Warning Pump')).toBeInTheDocument()
    expect(screen.queryByText('Pump')).not.toBeInTheDocument()
  })

  it('Deactivate butonu sadece admin için görünür ve çalışır', () => {
    localStorage.setItem('user_role', 'admin')
    const dispatchMock = vi.fn()
    vi.spyOn(MaintenanceContext, 'useMaintenanceDispatch').mockReturnValue(dispatchMock)
    vi.spyOn(MaintenanceContext, 'useMaintenance').mockReturnValue({
      machines: [{ id: 1, name: 'Pump', location: 'A1', status: 'Operational' }]
    })

    render(<MachineList />)

    const deactivateBtn = screen.getByText(/Deactivate/i)
    fireEvent.click(deactivateBtn)
    expect(dispatchMock).toHaveBeenCalledWith({ type: 'REMOVE_MACHINE', payload: 1 })
  })

  it('View sensors başarıyla data getirir', async () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance').mockReturnValue({
      machines: [{ id: 1, name: 'Pump', location: 'A1', status: 'Operational' }]
    })

    global.fetch = vi.fn(() =>
      Promise.resolve({
        json: () => Promise.resolve([
          { sensor_id: 1, type: 'temperature', value: 45, unit: 'C' }
        ])
      })
    )

    render(<MachineList />)

    fireEvent.click(screen.getByText(/VIEW SENSORS/i))

    await waitFor(() => {
      expect(screen.getByText(/TEMPERATURE/i)).toBeInTheDocument()
      expect(screen.getByText('45')).toBeInTheDocument()
    })
  })

  it('View sensors fetch hatası handle edilir', async () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance').mockReturnValue({
      machines: [{ id: 1, name: 'Pump', location: 'A1', status: 'Operational' }]
    })
    global.fetch = vi.fn().mockRejectedValue(new Error('Fetch error'))
    const consoleSpy = vi.spyOn(console, 'error').mockImplementation(() => { })

    render(<MachineList />)

    fireEvent.click(screen.getByText(/VIEW SENSORS/i))

    await waitFor(() => {
      expect(consoleSpy).toHaveBeenCalledWith("Sensor fetch error:", expect.any(Error))
    })
    consoleSpy.mockRestore()
  })
})
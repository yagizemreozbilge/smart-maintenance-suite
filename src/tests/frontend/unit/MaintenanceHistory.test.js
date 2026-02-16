import { render, screen } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { MaintenanceHistory } from '../../frontend/src/components/MaintenanceHistory'

vi.mock('../../frontend/src/context/MaintenanceContext', () => ({
  useMaintenance: () => ({
    maintenanceLogs: []
  })
}))

describe('MaintenanceHistory', () => {

  it('Boş history mesajı gösterir', () => {
    render(<MaintenanceHistory />)
    expect(screen.getByText(/No history records/i)).toBeInTheDocument()
  })

  it('Log varsa render edilir', () => {
    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenance')
      .mockReturnValue({
        maintenanceLogs: [
          {
            id: 1,
            date: '2025-01-01',
            performer: 'Admin',
            description: 'Oil change',
            machine_id: 1
          }
        ]
      })

    render(<MaintenanceHistory />)

    expect(screen.getByText(/Oil change/i)).toBeInTheDocument()
    expect(screen.getByText(/Admin/i)).toBeInTheDocument()
  })
})
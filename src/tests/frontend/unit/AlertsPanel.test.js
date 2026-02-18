import { render, screen } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { AlertsPanel } from '../../../frontend/src/components/AlertsPanel'
import * as MaintenanceContext from '../../../frontend/src/context/MaintenanceContext'

vi.mock('../../../frontend/src/context/MaintenanceContext', () => ({
  useMaintenance: () => ({
    alerts: []
  })
}))

describe('AlertsPanel', () => {

  it('Alert varsa render edilir', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        alerts: [
          {
            id: 1,
            severity: 'CRITICAL',
            message: 'Overheating detected',
            created_at: '2025-01-01'
          }
        ]
      })

    render(<AlertsPanel />)

    expect(screen.getByText('Overheating detected')).toBeInTheDocument()
    expect(screen.getByText('CRITICAL')).toBeInTheDocument()
  })

  it('Severity CRITICAL style branch çalışır', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        alerts: [
          {
            id: 2,
            severity: 'CRITICAL',
            message: 'Critical alert',
            created_at: '2025'
          }
        ]
      })

    render(<AlertsPanel />)

    const severityText = screen.getByText('CRITICAL')
    expect(severityText).toBeInTheDocument()
  })

  it('INFO severity branch çalışır', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        alerts: [
          {
            id: 3,
            severity: 'INFO',
            message: 'Info alert',
            created_at: '2025'
          }
        ]
      })

    render(<AlertsPanel />)

    expect(screen.getByText('INFO')).toBeInTheDocument()
  })
})
import { render, screen } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { AnalyticsPanel } from '../../frontend/src/components/AnalyticsPanel'

vi.mock('recharts', () => ({
  ResponsiveContainer: ({ children }) => <div>{children}</div>,
  PieChart: ({ children }) => <div>{children}</div>,
  Pie: ({ children }) => <div>{children}</div>,
  Cell: () => <div />,
  Tooltip: () => <div />,
  Legend: () => <div />,
  BarChart: ({ children }) => <div>{children}</div>,
  Bar: () => <div />,
  XAxis: () => <div />,
  YAxis: () => <div />,
  CartesianGrid: () => <div />
}))

vi.mock('../../frontend/src/context/MaintenanceContext', () => ({
  useMaintenance: () => ({
    machines: [],
    inventory: []
  })
}))

describe('AnalyticsPanel', () => {

  it('Component render edilir', () => {
    render(<AnalyticsPanel />)
    expect(screen.getByText(/Asset Status Distribution/i)).toBeInTheDocument()
    expect(screen.getByText(/Inventory Levels vs Min Stock/i)).toBeInTheDocument()
  })

  it('Data varsa render edilir', () => {
    vi.spyOn(require('../../frontend/src/context/MaintenanceContext'), 'useMaintenance')
      .mockReturnValue({
        machines: [
          { status: 'operational' },
          { status: 'warning' }
        ],
        inventory: [
          { part_name: 'Filter', quantity: 5, min_stock_level: 2 }
        ]
      })

    render(<AnalyticsPanel />)

    expect(screen.getByText(/Asset Status Distribution/i)).toBeInTheDocument()
  })
})
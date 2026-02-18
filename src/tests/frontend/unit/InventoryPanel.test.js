import { render, screen } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { InventoryPanel } from '../../../frontend/src/components/InventoryPanel'
import * as MaintenanceContext from '../../../frontend/src/context/MaintenanceContext'

vi.mock('../../../frontend/src/context/MaintenanceContext', () => ({
  useMaintenance: () => ({
    inventory: []
  })
}))

describe('InventoryPanel', () => {

  it('Empty inventory render edilir', () => {
    render(<InventoryPanel />)
    expect(screen.getByText(/Parts & Inventory/i)).toBeInTheDocument()
  })

  it('Inventory data render edilir', () => {
    vi.spyOn(MaintenanceContext, 'useMaintenance')
      .mockReturnValue({
        inventory: [
          {
            id: 1,
            part_name: 'Filter',
            sku: 'F-001',
            quantity: 5,
            min_stock: 2
          }
        ]
      })

    render(<InventoryPanel />)

    expect(screen.getByText('Filter')).toBeInTheDocument()
    expect(screen.getByText(/F-001/i)).toBeInTheDocument()
  })
})